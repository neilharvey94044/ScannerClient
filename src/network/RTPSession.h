// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+


#pragma once

#include <string>
#include <memory>
#include <span>
#include <atomic>
#include <future>
#include "audio/AudioBuffer.h"
#include "network/UDPSocket.h"


namespace sc{

typedef struct  {
    int             version;
    bool            padding;
    bool            extension;
    int             CSRCcount;
    bool            marker;
    int             payload_type;
    unsigned short  sequence;
    unsigned long    timestamp;
    unsigned long    SSRC;        //Synchronization Source
} RTPHDRexpanded;

class RTPSession {
    public:

        RTPSession(std::string scanner_ip, int listen_port, std::shared_ptr<AudioBuffer>& audio_buf_ptr);
        void start(std::promise<bool> rtp_success_promise);
        void stop();

    private:
        void execute(std::promise<bool> rtp_success_promise);
        void writeraw(std::string dgram);
        RTPHDRexpanded formatHeader(std::span<char> dgram);
        UDPSocket m_rtp_sock;
        std::shared_ptr<AudioBuffer> m_audio_buf_ptr;
        std::unique_ptr<std::thread> m_rtp_thread{nullptr};
        std::atomic_bool m_running{false};

};

} // namespace