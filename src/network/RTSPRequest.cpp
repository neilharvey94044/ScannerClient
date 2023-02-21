// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+


#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <string>
#include <iostream>
#include "network/RTSPRequest.h"

using namespace std;

namespace sc {

string RTSPRequest::getString() const {
    spdlog::debug("Entering RTSPRequest::getString()");

    switch (m_method){
        case RTSPMethod::OPTIONS:
            return              fmt::format("OPTIONS rtsp://{}:{}/au:scanner.au RTSP/1.0" "\r\n", m_server_ip, m_server_port) +
                                fmt::format("CSeq: {}" "\r\n", m_cseq) +
                                fmt::format("User-Agent: {}" "\r\n\r\n", m_user_agent);
            break;
        case RTSPMethod::DESCRIBE:  
            return              fmt::format("DESCRIBE rtsp://{}:{}/au:scanner.au RTSP/1.0" "\r\n", m_server_ip, m_server_port) +
                                fmt::format("CSeq: {}" "\r\n", m_cseq) +
                                fmt::format("User-Agent: {}" "\r\n", m_user_agent) +
                                            "Accept: application/sdp" "\r\n\r\n";
            break;
        case RTSPMethod::SETUP:
            return              fmt::format("SETUP rtsp://{}:{}/au:scanner.au/{} RTSP/1.0" "\r\n", m_server_ip, m_server_port, m_audio_channel) +
                                fmt::format("CSeq: {}" "\r\n", m_cseq) +
                                fmt::format("User-Agent: {}" "\r\n", m_user_agent) +
                                fmt::format("Transport: RTP/AVP;unicast;client_port={}" "\r\n\r\n", m_hint_rtp_port);
            break;
        case RTSPMethod::PLAY:
             return             fmt::format("PLAY rtsp://{}:{}/au:scanner.au/ RTSP/1.0" "\r\n", m_server_ip, m_server_port) +
                                fmt::format("CSeq: {}" "\r\n", m_cseq) +
                                fmt::format("User-Agent: {}" "\r\n", m_user_agent) +
                                fmt::format("Session: {}" "\r\n", m_session_id) + 
                                            "Range: npt=0.000-" "\r\n\r\n";
            break;
        case RTSPMethod::TEARDOWN:
             return             fmt::format("TEARDOWN rtsp://{}:{}/au:scanner.au/ RTSP/1.0" "\r\n", m_server_ip, m_server_port) +
                                fmt::format("CSeq: {}" "\r\n", m_cseq) +
                                fmt::format("User-Agent: {}" "\r\n", m_user_agent) +
                                fmt::format("Session: {}" "\r\n\r\n", m_session_id);

            break;
        case RTSPMethod::KEEPALIVE:
             return             fmt::format("GET_PARAMETER rtsp://{}:{}/au:scanner.au/ RTSP/1.0" "\r\n", m_server_ip, m_server_port) +
                                fmt::format("CSeq: {}" "\r\n", m_cseq) +
                                fmt::format("User-Agent: {}" "\r\n", m_user_agent) +
                                fmt::format("Session: {}" "\r\n\r\n", m_session_id);

            break;
        default:
            return "";
    }

}

void RTSPRequest::setSessionId(const string session_id) {m_session_id = session_id;}
void RTSPRequest::setHintRTPPort(const string hint_rtp_port) {m_hint_rtp_port = hint_rtp_port;}
void RTSPRequest::setAudioChannel(const string audio_channel) {m_audio_channel = audio_channel;}


} //namespace
