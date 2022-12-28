// Copyright (c) Neil D. Harvey

#pragma once

#include <string>
#include <memory>
#include <network/Socket.h>

namespace sc {


class UDPSocket : public sc::Socket{
    public:
        virtual int sendto(std::string);
        virtual std::string recvfrom();
        UDPSocket(std::string scanner_ip, int scanner_port);
        virtual ~UDPSocket();

    private:
        char m_msgin[4096];
        struct sockaddr_in m_server_addr;
        struct sockaddr_in m_in_address;
        socklen_t m_in_address_len = sizeof(m_in_address);

};

} // sc