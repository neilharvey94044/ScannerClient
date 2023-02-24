// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once


#include <string>
#include <memory>
#include <config/SC_CONFIG.h>
#include "network/Socket.h"

namespace sc {


class TCPSocket : public sc::Socket{
    public:
        TCPSocket(std::string scanner_ip, int scanner_port);
        virtual int connect();
        virtual int send(std::string);
        virtual std::string recv();
        virtual ~TCPSocket();
    
    private:
        std::unique_ptr<SC_CONFIG> m_pConfig{nullptr};
        std::string m_scanner_ip{};
        int m_scanner_port{};
        char m_msgin[4096];
        struct sockaddr_in m_server_addr;
        struct sockaddr_in m_in_address;
        socklen_t m_in_address_len = sizeof(m_in_address);
};

} // sc