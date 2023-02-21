// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+


#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include "network/RTSPRequest.h"
#include "network/RTSPResponse.h"
#include "network/RTPSession.h"
#include "network/TCPSocket.h"
#include "audio/AudioBuffer.h"
#include "network/RTSPSession.h"
#include "config/SC_CONFIG.h"

using namespace std;

namespace sc{

RTSPSession::RTSPSession(): 
            m_pConfig{SC_CONFIG::get()} 
{

    m_server_ip = m_pConfig->ip_address;
    m_server_port = m_pConfig->audio_rtsp_port;
    m_tcp_socket_ptr =  std::make_unique<TCPSocket>(m_server_ip, m_server_port);
    m_audio_buf_ptr = std::make_shared<AudioBuffer>(5);
}


void RTSPSession::start(std::promise<bool> rtsp_success_promise){

        m_rtsp_thread = make_unique<std::thread>(&RTSPSession::execute, this, std::move(rtsp_success_promise));
        spdlog::debug("Exiting RTSPSession::start()");

}

void RTSPSession::execute(std::promise<bool> rtsp_success_promise){
    spdlog::debug("RTSPSession::execute entered");

    spdlog::debug("Connecting to {}:{}", m_server_ip, m_server_port);

    if (m_tcp_socket_ptr->connect() == SOCKET_ERROR){
        spdlog::error("Error connecting - RTSPSession thread terminated");
        m_running = false;
        m_audio_buf_ptr->setStopped(true);
        rtsp_success_promise.set_value(false);
        return;
    }
    spdlog::debug("RTSPSession after connecting TCP socket");

    m_running = true;
    m_seq = 2;
    m_last_status = sendOPTIONS();
    if(m_last_status == 200) m_last_status = sendDESCRIBE();
    if(m_last_status == 200) m_last_status = sendSETUP();
    if(m_last_status == 200) m_last_status = sendPLAY();

    if(m_tcp_socket_ptr->getPollReturn() == Socket::POLLRET::STIMEOUT){
        spdlog::error("RTSPSession thread network timeout.");
        m_running = false;
        m_audio_buf_ptr->setStopped(true);
        rtsp_success_promise.set_value(false);
        return;
    }
    
    // receive PCMU audio over RTP and populate AudioBuffer
    //TODO: put Promise/Future in here to understand success/failure of RTP Session startup
    if(m_last_status == 200){
        m_rtp_session_ptr = make_unique<RTPSession>(m_server_ip, m_actual_rtp_port, m_audio_buf_ptr);

        std::promise<bool> rtp_success_promise;
        std::future<bool> rtp_success_future = rtp_success_promise.get_future();
        m_rtp_session_ptr->start(std::move(rtp_success_promise));
        if(!rtp_success_future.get()){
            m_rtp_session_ptr->stop();
            m_running = false;
            m_audio_buf_ptr->setStopped(true);
            rtsp_success_promise.set_value(false);
            return;
        }
    }

    //TODO put here until RTP session is similarly tracked with promise/future
    rtsp_success_promise.set_value(true);

    // send keepalive to keep connection open
    while(m_running){
        spdlog::debug("Top of RTSPSession while loop");
        using namespace std::chrono_literals;
        std::unique_lock<std::mutex> lk(m_cv_m);
        auto now = std::chrono::system_clock::now();
        // sleep for 40 seconds or until signaled to stop
        m_cv.wait_until(lk, now + 40s);

        if(m_running){
            if (sendKEEPALIVE() != 200 || m_tcp_socket_ptr->getPollReturn() == Socket::POLLRET::STIMEOUT){
                spdlog::error("Bad response or timeout on RTSP KeepAlive");
                m_running = false;
            }
        }
    }
    spdlog::debug("Exited RTSPSession while loop");
    m_rtp_session_ptr->stop();
    sendTEARDOWN();

}

std::shared_ptr<AudioBuffer> RTSPSession::getAudioBuffer(){
    return m_audio_buf_ptr;
}

bool RTSPSession::isRunning() const{
    return m_running;
}


// will stop the thread
void RTSPSession::stop(){
    spdlog::info("RTSPSession::stop invoked");
    if(m_running){
        m_running = false;
        m_cv.notify_one();
    }
    if(m_rtsp_thread != nullptr && m_rtsp_thread->joinable()){
        m_rtsp_thread->join();
    }
}

RTSPSession::~RTSPSession(){
    spdlog::debug("Destroying RTSPSession object");

}



int RTSPSession::sendOPTIONS(){
    auto reqOPTIONS = make_unique<RTSPRequest>(RTSPMethod::OPTIONS, m_server_ip, m_pConfig->user_agent, m_seq++);
    m_tcp_socket_ptr->send(reqOPTIONS->getString());

    std::string respOPTIONSstr = m_tcp_socket_ptr->recv();
    RTSPResponse respOPTIONS(RTSPMethod::OPTIONS, respOPTIONSstr);
    return respOPTIONS.getStatus();
}

int RTSPSession::sendDESCRIBE(){
    auto reqDESCRIBE = make_unique<RTSPRequest>(RTSPMethod::DESCRIBE, m_server_ip, m_pConfig->user_agent, m_seq++);
    m_tcp_socket_ptr->send(reqDESCRIBE->getString());

    std::string respDESCRIBEstr = m_tcp_socket_ptr->recv();
    RTSPResponse respDESCRIBE(RTSPMethod::DESCRIBE, respDESCRIBEstr);
    m_audio_channel = respDESCRIBE.getAudioChannel();
    return respDESCRIBE.getStatus();
}
int RTSPSession::sendSETUP(){
    auto reqSETUP = make_unique<RTSPRequest>(RTSPMethod::SETUP, m_server_ip, m_pConfig->user_agent, m_seq++);
    reqSETUP->setHintRTPPort(m_pConfig->hint_rtp_port);  //the requested UDP port to listen on for RTP stream
    reqSETUP->setAudioChannel(m_audio_channel);
    m_tcp_socket_ptr->send(reqSETUP->getString());

    std::string respSETUPstr = m_tcp_socket_ptr->recv();
    RTSPResponse respSETUP(RTSPMethod::SETUP, respSETUPstr);
    m_current_sessionID = respSETUP.getSession();
    m_actual_rtp_port = respSETUP.getRTPPort();
    return respSETUP.getStatus();

  
}
int RTSPSession::sendPLAY(){
    auto reqPLAY = make_unique<RTSPRequest>(RTSPMethod::PLAY, m_server_ip, m_pConfig->user_agent, m_seq++);
    reqPLAY->setSessionId(m_current_sessionID); 
    m_tcp_socket_ptr->send(reqPLAY->getString());

    // PLAY - receive
    std::string respPLAYstr = m_tcp_socket_ptr->recv();
    RTSPResponse respPLAY(RTSPMethod::PLAY, respPLAYstr);
    return respPLAY.getStatus();
}
void RTSPSession::sendTEARDOWN(){
    spdlog::info("Sending TEARDOWN");
    auto reqTEARDOWN = make_unique<RTSPRequest>(RTSPMethod::TEARDOWN, m_server_ip, m_pConfig->user_agent, m_seq++);
    reqTEARDOWN->setSessionId(m_current_sessionID);
    m_tcp_socket_ptr->send(reqTEARDOWN->getString());
}

// needs to be invoked every 40 seconds to keep the connection alive
int RTSPSession::sendKEEPALIVE(){
    spdlog::info("Sending KEEPALIVE");
    auto reqKEEPALIVE = make_unique<RTSPRequest>(RTSPMethod::KEEPALIVE, m_server_ip, m_pConfig->user_agent, m_seq++);
    reqKEEPALIVE->setSessionId(m_current_sessionID);
    m_tcp_socket_ptr->send(reqKEEPALIVE->getString());

        // KEEPALIVE - receive
    std::string respKEEPALIVEstr = m_tcp_socket_ptr->recv();
    RTSPResponse respKEEPALIVE(RTSPMethod::KEEPALIVE, respKEEPALIVEstr);
    return respKEEPALIVE.getStatus();
}

} //namespace