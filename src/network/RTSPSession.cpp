// Copyright (c) Neil D. Harvey


#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include "scannerclient/RTSPRequest.h"
#include "scannerclient/RTSPResponse.h"
#include "scannerclient/RTPSession.h"
#include "scannerclient/TCPSocket.h"
#include "audio/AudioBuffer.h"
#include "scannerclient/sc.h"
#include "scannerclient/RTSPSession.h"

using namespace std;

namespace sc{




void RTSPSession::start(){

        m_rtsp_thread = make_unique<std::thread>(&RTSPSession::execute, this);

}

void RTSPSession::execute(){
    spdlog::debug("RTSPSession::execute entered");
    m_running = true;
    m_tcp_socket.connect();

    m_seq = 2;
    m_last_status = sendOPTIONS();
    if(m_last_status == 200) m_last_status = sendDESCRIBE();
    if(m_last_status == 200) m_last_status = sendSETUP();
    if(m_last_status == 200) m_last_status = sendPLAY();

    
    if(m_last_status == 200){
        // receive PCMU audio over RTP and populate AudioBuffer

        m_rtp_session_ptr = make_unique<RTPSession>(m_server_ip, m_actual_rtp_port, m_audio_buf_ptr);
        m_rtp_session_ptr->start();
    }

    while(m_running){
        spdlog::debug("Top of RTSPSession while loop");
        using namespace std::chrono_literals;
        std::unique_lock<std::mutex> lk(m_cv_m);
        auto now = std::chrono::system_clock::now();
        // sleep for 40 seconds or until signaled to stop
        m_cv.wait_until(lk, now + 40s);

        if(m_running){
            if (sendKEEPALIVE() != 200){
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


// will stop the thread
void RTSPSession::stop(){
    spdlog::info("RTSPSession::stop invoked");
    m_running = false;
    m_cv.notify_one();
    m_rtsp_thread->join();
}

RTSPSession::~RTSPSession(){
    spdlog::debug("Destroying RTSPSession object");

}



int RTSPSession::sendOPTIONS(){
    auto reqOPTIONS = make_unique<RTSPRequest>(RTSPMethod::OPTIONS, m_server_ip, SC_CONFIG.user_agent, m_seq++);
    m_tcp_socket.send(reqOPTIONS->getString());

    std::string respOPTIONSstr = m_tcp_socket.recv();
    RTSPResponse respOPTIONS(RTSPMethod::OPTIONS, respOPTIONSstr);
    return respOPTIONS.getStatus();
}

int RTSPSession::sendDESCRIBE(){
    auto reqDESCRIBE = make_unique<RTSPRequest>(RTSPMethod::DESCRIBE, m_server_ip, SC_CONFIG.user_agent, m_seq++);
    m_tcp_socket.send(reqDESCRIBE->getString());

    std::string respDESCRIBEstr = m_tcp_socket.recv();
    RTSPResponse respDESCRIBE(RTSPMethod::DESCRIBE, respDESCRIBEstr);
    m_audio_channel = respDESCRIBE.getAudioChannel();
    return respDESCRIBE.getStatus();
}
int RTSPSession::sendSETUP(){
    auto reqSETUP = make_unique<RTSPRequest>(RTSPMethod::SETUP, m_server_ip, SC_CONFIG.user_agent, m_seq++);
    reqSETUP->setHintRTPPort(SC_CONFIG.hint_rtp_port);  //the requested UDP port to listen on for RTP stream
    reqSETUP->setAudioChannel(m_audio_channel);
    m_tcp_socket.send(reqSETUP->getString());

    std::string respSETUPstr = m_tcp_socket.recv();
    RTSPResponse respSETUP(RTSPMethod::SETUP, respSETUPstr);
    m_current_sessionID = respSETUP.getSession();
    m_actual_rtp_port = respSETUP.getRTPPort();
    return respSETUP.getStatus();

  
}
int RTSPSession::sendPLAY(){
    auto reqPLAY = make_unique<RTSPRequest>(RTSPMethod::PLAY, m_server_ip, SC_CONFIG.user_agent, m_seq++);
    reqPLAY->setSessionId(m_current_sessionID); 
    m_tcp_socket.send(reqPLAY->getString());

    // PLAY - receive
    std::string respPLAYstr = m_tcp_socket.recv();
    RTSPResponse respPLAY(RTSPMethod::PLAY, respPLAYstr);
    return respPLAY.getStatus();
}
void RTSPSession::sendTEARDOWN(){
    spdlog::info("Sending TEARDOWN");
    auto reqTEARDOWN = make_unique<RTSPRequest>(RTSPMethod::TEARDOWN, m_server_ip, SC_CONFIG.user_agent, m_seq++);
    reqTEARDOWN->setSessionId(m_current_sessionID);
    m_tcp_socket.send(reqTEARDOWN->getString());
}

// needs to be invoked every 40 seconds to keep the connection alive
int RTSPSession::sendKEEPALIVE(){
    spdlog::info("Sending KEEPALIVE");
    auto reqKEEPALIVE = make_unique<RTSPRequest>(RTSPMethod::KEEPALIVE, m_server_ip, SC_CONFIG.user_agent, m_seq++);
    reqKEEPALIVE->setSessionId(m_current_sessionID);
    m_tcp_socket.send(reqKEEPALIVE->getString());

        // KEEPALIVE - receive
    std::string respKEEPALIVEstr = m_tcp_socket.recv();
    RTSPResponse respKEEPALIVE(RTSPMethod::KEEPALIVE, respKEEPALIVEstr);
    return respKEEPALIVE.getStatus();
}

} //namespace