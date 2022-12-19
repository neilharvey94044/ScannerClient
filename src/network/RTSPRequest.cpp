// Copyright (c) Neil D. Harvey

#include <spdlog/spdlog.h>
#include <string>
#include <network/RTSPRequest.h>

namespace sc {


string RTSPRequest::getString() const {

    switch (m_method){
        case RTSPMethod::OPTIONS:
            return fmt::format(rtspHdr+rtspEnd, "OPTIONS", 
                                m_server_ip, ":"+ m_server_port, "", m_cseq, m_user_agent);
            break;
        case RTSPMethod::DESCRIBE:  
            return fmt::format(rtspHdr+rtspCR+rtspAttrAccept+rtspEnd, "DESCRIBE", 
                                m_server_ip, ":"+m_server_port, "", m_cseq, m_user_agent);
            break;
        case RTSPMethod::SETUP:
            return fmt::format(rtspHdr+rtspCR+rtspAttrTransport+rtspEnd, "SETUP", 
                                m_server_ip, ":"+m_server_port, "/trackID=1", m_cseq, m_user_agent, m_rtp_port);
            break;
        case RTSPMethod::PLAY:
            return fmt::format(rtspHdr+rtspCR+rtspAttrSession+rtspCR+rtspAttrRange+rtspEnd, "PLAY", 
                                m_server_ip, ":"+m_server_port, "/", m_cseq, m_user_agent, m_session_id);
            break;
        case RTSPMethod::TEARDOWN:
            return fmt::format(rtspHdr+rtspCR+rtspAttrSession+rtspEnd, "TEARDOWN", 
                                m_server_ip, ":"+m_server_port, "/", m_cseq, m_user_agent, m_session_id);

            break;
        default:
            return "";
    }

}

void RTSPRequest::setSessionId(const string session_id) {m_session_id = session_id;}
void RTSPRequest::setRTPPort(const string rtp_port) {m_rtp_port = rtp_port;}


} //namespace
