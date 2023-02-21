// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include <string>
#include <memory>
#include "config/SC_CONFIG.h"
#include "network/Socket.h"

namespace sc {


class UDPSocket : public sc::Socket{
    public:
        UDPSocket(std::string scanner_ip, int scanner_port);
        UDPSocket(std::string scanner_ip, int scanner_port, int listen_port );
        virtual int bind();
        virtual int sendto(std::string);
        //TODO: give this a graceful timeout - "select()?"
        virtual std::string recvfrom();
        virtual ~UDPSocket();

    private:
        std::string m_scanner_ip{};
        int m_scanner_port{};
        int m_listen_port{};
        char m_msgin[4096];
        std::unique_ptr<SC_CONFIG> m_pConfig{nullptr};
        struct sockaddr_in m_server_addr;
        struct sockaddr_in m_in_address;
        socklen_t m_in_address_len = sizeof(m_in_address);

};

} // sc