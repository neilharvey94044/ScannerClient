// Copyright (c) Neil D. Harvey
// 
// Note:  This class specifically supports the Uniden scanner, it's not generalized.

#pragma once

#include <unordered_map>
#include <string>

using namespace std;

namespace sc {

// limited to the rtsp method types needed for the Uniden scanner
enum RTSPMethod {
            OPTIONS,
            DESCRIBE,
            SETUP,
            PLAY,
            TEARDOWN
        };

class RTSPRequest {
    public:

         RTSPRequest(const RTSPMethod method, const string server_ip, const string user_agent, const int cseq) 
        :   m_method {method},
            m_server_ip {server_ip},
            m_user_agent {user_agent},
            m_cseq {cseq}
        {};
         RTSPRequest(const RTSPMethod method, const string server_ip, const string server_port, const string user_agent, const int cseq) 
        :   m_method {method},
            m_server_ip {server_ip},
            m_server_port {server_port},
            m_user_agent {user_agent},
            m_cseq {cseq}
        {};
        void setSessionId(const string session_id);
        void setRTPPort(const string rtp_port);
        string getString() const;



    protected:
        RTSPMethod m_method;
        string m_user_agent;
        string m_session_id;
        string m_server_ip;
        string m_server_port {"554"};
        string m_rtp_port {"49990-49991"};
        int m_cseq;
        const string rtspEnd              {"\r\n\r\n"};
        const string rtspCR               {"\r\n"};
        const string rtspAttrAccept       {"Accept: application/sdp"};
        const string rtspAttrTransport    {"Transport: RTP/AVP;unicast;client_port={}"};
        const string rtspAttrSession      {"Session: {}"};
        const string rtspAttrRange        {"Range: npt=0.000-"};
        const string rtspHdr              {"{} rtsp://{}{}/au:scanner.au{} RTSP/1.0"+ rtspCR +"CSeq: {}"+rtspCR+"User-Agent: {}"};
};

} //namespace