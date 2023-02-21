// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include <string>
#include <memory>
#include <future>
#include "network/RTPSession.h"
#include "audio/AudioBuffer.h"
#include "network/TCPSocket.h"
#include "config/SC_CONFIG.h"

namespace sc{

class RTSPSession {

    public:
        RTSPSession();
        void start(std::promise<bool> rtsp_success_promise);
        void stop();
        std::shared_ptr<AudioBuffer> getAudioBuffer();
        bool isRunning() const;
        virtual ~RTSPSession();

    private:
        std::string m_server_ip{};
        int m_server_port{0};
        std::unique_ptr<SC_CONFIG> m_pConfig{nullptr};
        std::unique_ptr<TCPSocket> m_tcp_socket_ptr;
        int m_seq{0};
        std::string m_audio_channel{};
        std::string m_current_sessionID{};
        int m_actual_rtp_port{0};
        int m_last_status{0};
        std::atomic_bool m_running{false};
        std::unique_ptr<RTPSession> m_rtp_session_ptr{nullptr};
        std::shared_ptr<AudioBuffer>m_audio_buf_ptr{};
        std::unique_ptr<std::thread>m_rtsp_thread{nullptr};
        std::condition_variable m_cv{};
        std::mutex m_cv_m{};



        void execute(std::promise<bool> rtsp_success_promise);

        int sendOPTIONS();
        int sendDESCRIBE();
        int sendSETUP();
        int sendPLAY();
        void sendTEARDOWN();
        int sendKEEPALIVE();

};

} //namespace