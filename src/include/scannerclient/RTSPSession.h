// Copyright (c) Neil D. Harvey

#pragma once

#include <string>
#include <memory>
#include "scannerclient/RTPSession.h"
#include "audio/AudioBuffer.h"
#include "scannerclient/TCPSocket.h"
#include "scannerclient/sc.h"

namespace sc{

class RTSPSession {

    public:
        RTSPSession():
            m_server_ip {SC_CONFIG.ip_address},
            m_server_port{SC_CONFIG.audio_rtsp_port},
            m_tcp_socket{m_server_ip, m_server_port},
            m_audio_buf_ptr{std::make_shared<AudioBuffer>(5)}
            {};
        void start();
        void stop();
        std::shared_ptr<AudioBuffer> getAudioBuffer();
        virtual ~RTSPSession();

    private:
        std::string m_server_ip{};
        int m_server_port{0};
        TCPSocket m_tcp_socket;
        int m_seq{0};
        std::string m_audio_channel{};
        std::string m_current_sessionID{};
        int m_actual_rtp_port{0};
        int m_last_status{0};
        std::atomic_bool m_running{false};
        std::unique_ptr<RTPSession> m_rtp_session_ptr;
        std::shared_ptr<AudioBuffer>m_audio_buf_ptr;
        std::unique_ptr<std::thread>m_rtsp_thread;
        std::condition_variable m_cv;
        std::mutex m_cv_m;



        void execute();

        int sendOPTIONS();
        int sendDESCRIBE();
        int sendSETUP();
        int sendPLAY();
        void sendTEARDOWN();
        int sendKEEPALIVE();

};

} //namespace