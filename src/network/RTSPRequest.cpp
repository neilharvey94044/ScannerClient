// Copyright (c) Neil D. Harvey

#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <string>
#include <scannerclient/RTSPRequest.h>

namespace sc {


string RTSPRequest::getString() const {
    spdlog::debug("Entering RTSPRequest::getString()");

    switch (m_method){
        case RTSPMethod::OPTIONS:
            return fmt::format("OPTIONS rtsp://{}:{}/au:scanner.au RTSP/1.0" "\r\n" "CSeq: {}" "\r\n" "User-Agent: {}" "\r\n\r\n",
                                m_server_ip, m_server_port, m_cseq, m_user_agent);
            break;
        case RTSPMethod::DESCRIBE:  
            return fmt::format("DESCRIBE rtsp://{}:{}/au:scanner.au RTSP/1.0" "\r\n" "CSeq: {}" "\r\n" "" "Accept: application/sdp" "\r\n\r\n",
                                m_server_ip, m_server_port, m_cseq, m_user_agent);
            break;
        case RTSPMethod::SETUP:
            return fmt::format("SETUP rtsp://{}:{}/au:scanner.au RTSP/1.0/{}" "\r\n" "CSeq: {}" "\r\n" "Transport: RTP/AVP;unicast;client_port={}" "\r\n\r\n",
                                m_server_ip, m_server_port, "trackID=1", m_cseq, m_user_agent, m_rtp_port);
            break;
        case RTSPMethod::PLAY:
             return fmt::format("PLAY rtsp://{}:{}/au:scanner.au RTSP/1.0/" "\r\n" "CSeq: {}" "\r\n" "Session: {}" "\r\n" "Range: npt=0.000-" "\r\n\r\n",
                                m_server_ip, m_server_port, m_cseq, m_user_agent, m_session_id);
            break;
        case RTSPMethod::TEARDOWN:
             return fmt::format("TEARDOWN rtsp://{}:{}/au:scanner.au RTSP/1.0/" "\r\n" "CSeq: {}" "\r\n" "Session: {}" "\r\n\r\n",
                                m_server_ip, m_server_port, m_cseq, m_user_agent, m_session_id);

            break;
        default:
            return "";
    }

}

void RTSPRequest::setSessionId(const string session_id) {m_session_id = session_id;}
void RTSPRequest::setRTPPort(const string rtp_port) {m_rtp_port = rtp_port;}


} //namespace
