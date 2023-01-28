// Copyright (c) Neil D. Harvey


#pragma once

#include <string>
#include <memory>
#include <span>
#include <atomic>
#include "audio/AudioBuffer.h"
#include "scannerclient/UDPSocket.h"


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
        void start();
        void stop();

    private:
        void execute();
        void writeraw(std::string dgram);
        RTPHDRexpanded formatHeader(std::span<char> dgram);
        UDPSocket m_rtp_sock;
        std::shared_ptr<AudioBuffer> m_audio_buf_ptr;
        std::unique_ptr<std::thread> m_rtp_thread;
        std::atomic_bool m_running{false};

};

} // namespace