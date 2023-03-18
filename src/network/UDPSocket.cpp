// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <memory>
#include <string>
#include <span>
#include <spdlog/spdlog.h>
#include "network/UDPSocket.h"

namespace sc {

// Used to get request/response scanner status
UDPSocket::UDPSocket(std::string scanner_ip, int scanner_port):
            m_scanner_ip{scanner_ip},
            m_scanner_port{scanner_port},
            Socket(SOCK_DGRAM, IPPROTO_UDP),
            m_pConfig(SC_CONFIG::get())
            { }

// Used to receive RTP PCMU audio
// Parameter scanner_port can be zero for now
UDPSocket::UDPSocket(std::string scanner_ip, int scanner_port, int listen_port):
            m_scanner_ip{scanner_ip},
            m_scanner_port{scanner_port},
            m_listen_port{listen_port},
            Socket(SOCK_DGRAM, IPPROTO_UDP)
            { }

int UDPSocket::bind(){

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, std::to_string(m_listen_port).c_str(), &hints, &bind_address);

    if (::bind(m_socket, bind_address->ai_addr, bind_address->ai_addrlen)) {
        spdlog::error("bind() failed with Socket Error {}", GETSOCKETERRNO());
        return 0;
    }

    return 1;
}

int UDPSocket::sendto(std::string msgout){
            m_server_addr.sin_family = AF_INET;
            m_server_addr.sin_port = htons(m_scanner_port);
            inet_pton(AF_INET, m_scanner_ip.c_str(), &(m_server_addr.sin_addr));

            spdlog::debug("Sending: {}", msgout);
            int bytes_sent = ::sendto(m_socket, msgout.c_str(), msgout.length(), 0, (const struct sockaddr *) &m_server_addr, sizeof(m_server_addr));
            if (bytes_sent < 0) {
                spdlog::error("sendto() failed with {} to address [{}]", GETSOCKETERRNO(), m_scanner_ip);
            }
            spdlog::debug("Sent {} bytes.", bytes_sent);

            return bytes_sent;
        }

std::string UDPSocket::recvfrom() {
            int pollresponse{0};
            pollresponse = pollForRead();
            if(pollresponse == Socket::POLLRET::STIMEOUT){
                spdlog::error("No data available after {} ms", m_pConfig->socket_read_wait_ms);
                return "";
            }

            memset(&m_msgin, 0x00, sizeof(m_msgin));
            int bytes_received = ::recvfrom(m_socket, m_msgin, sizeof(m_msgin), 0, (struct sockaddr *) &m_in_address, &m_in_address_len);
            if (bytes_received < 1) {
                spdlog::error("recvfrom() failed. {}", GETSOCKETERRNO());
            }
            spdlog::debug("Received ({} bytes)", bytes_received);
            return std::string(m_msgin, bytes_received);
        }

UDPSocket::~UDPSocket(){ }

} // namespace