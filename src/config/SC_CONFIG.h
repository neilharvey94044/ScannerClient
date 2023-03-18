// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include <string>
#include <memory>

// should not be tied to wxWidgets; separate interface and implementation
// follow dependency injection pattern
// available easily and globally from anywhere in the code


namespace sc {

class SC_CONFIG {

    public:
        static const std::unique_ptr<SC_CONFIG> get();
        virtual void load() = 0;

        std::string ip_address{"0.0.0.0"};
        long auto_start{0};
        std::string user_agent{"ScannerClient 1.0"};
        int audio_rtsp_port{554};
        int status_udp_port{50536};
        int socket_read_wait_ms{3000};
        int socket_write_wait_ms{3000};  
        std::string hint_rtp_port{"49990-49991"};
        long debug_logging{0};

};


} //namespace