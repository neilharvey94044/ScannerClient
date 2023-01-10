// Copyright (c) Neil D. Harvey


#pragma once

#include <string>
#include "scannerclient/UDPSocket.h"


namespace sc{

class RTPSession {
    public:
        RTPSession(std::string scanner_ip, int listen_port);
        void run();

    private:
        void expand_g711(std::string pcmu_in, std::span<short> pcm_out);
        void writeraw(std::string packet);
        UDPSocket m_rtp_sock;

};

} // namespace