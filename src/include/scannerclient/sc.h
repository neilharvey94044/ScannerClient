// Copyright (c) Neil D. Harvey

#pragma once

#include <string>

namespace sc{
//TODO: create method to populate and cache these values, remove default IP
//TODO: go through entire system to ensure only these values are being used
static struct {
    std::string user_agent{"ScannerClient 1.0"};
    std::string ip_address{"192.168.0.173"};
    int audio_rtsp_port{554};
    int status_udp_port{50536};
    std::string hint_rtp_port{"49990-49991"};

} SC_CONFIG;

typedef std::array<unsigned short, 320> rtpbuf;

} //namespace