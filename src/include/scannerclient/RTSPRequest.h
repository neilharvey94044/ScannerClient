// Copyright (c) Neil D. Harvey
// 
// Note:  This class specifically supports the Uniden scanner, it's not generalized.

#pragma once

#include <unordered_map>
#include <string>

namespace sc {

// limited to the rtsp method types needed for the Uniden scanner
enum RTSPMethod {
            OPTIONS,
            DESCRIBE,
            SETUP,
            PLAY,
            TEARDOWN,
            KEEPALIVE
        };

class RTSPRequest {
    public:

         RTSPRequest(const RTSPMethod method, const std::string server_ip, const std::string user_agent, const int cseq) 
        :   m_method {method},
            m_server_ip {server_ip},
            m_user_agent {user_agent},
            m_cseq {cseq}
        {};
         RTSPRequest(const RTSPMethod method, const std::string server_ip, const std::string server_port, const std::string user_agent, const int cseq) 
        :   m_method {method},
            m_server_ip {server_ip},
            m_server_port {server_port},
            m_user_agent {user_agent},
            m_cseq {cseq}
        {};
        void setSessionId(const std::string session_id);
        void setHintRTPPort(const std::string hint_rtp_port);
        void setAudioChannel(const std::string audio_channel);
        std::string getString() const;



    private:
        RTSPMethod m_method;
        std::string m_user_agent;
        std::string m_session_id;
        std::string m_server_ip;
        std::string m_audio_channel;
        std::string m_server_port {"554"};
        std::string m_hint_rtp_port {""};
        int m_cseq;

};

} //namespace