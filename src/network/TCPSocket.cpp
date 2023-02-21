// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include "config/SC_CONFIG.h"
#include "network/TCPSocket.h"

namespace sc {

TCPSocket::TCPSocket(std::string scanner_ip, int scanner_port) :
            m_scanner_ip {scanner_ip},
            m_scanner_port{scanner_port},
            Socket(SOCK_STREAM, IPPROTO_TCP),
            m_pConfig(SC_CONFIG::get())
        { 

            int flag = 1, sockerr = 0;

            sockerr = setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (char *) &flag, sizeof(flag) );
            if (sockerr != 0) {
                spdlog::error("setsockopt() failed. {}", GETSOCKETERRNO());
            }
        }

int TCPSocket::connect() {

    spdlog::debug("Connecting");

    //TODO: this is Windows only code
    // make socket non-blocking
    int iResult{0};
    unsigned long non_blocking = -1;
    iResult = ioctlsocket(m_socket, FIONBIO, &non_blocking);
    if (iResult != NO_ERROR){
        spdlog::error("ioctlsocket change to non-blocking failed with error: {}", iResult);
        return iResult;
    }

    // attempt to async connect to scanner
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(m_scanner_port);
    inet_pton(AF_INET, m_scanner_ip.c_str(), &(m_server_addr.sin_addr));
    iResult = ::connect(m_socket, (const struct sockaddr *) &m_server_addr, sizeof(m_server_addr));
        if((iResult == SOCKET_ERROR) && (GETSOCKETERRNO() != WSAEWOULDBLOCK)){
        spdlog::error("connect() failed. {}", GETSOCKETERRNO());
        return iResult;
    }

    int pollresponse{0};
    pollresponse = pollForWrite();
    if(pollresponse == Socket::POLLRET::STIMEOUT){
        spdlog::error("Unable to connect to scanner after {} ms", m_pConfig->socket_write_wait_ms);
        return SOCKET_ERROR;
    }

    // make socket blocking again
    unsigned long blocking = 0;
    iResult = ioctlsocket(m_socket, FIONBIO, &blocking);
    if (iResult != NO_ERROR){
        spdlog::error("ioctlsocket change to blocking failed with error: {}", iResult);
        return iResult;
    }
    return iResult;
}

int TCPSocket::send(std::string msgout) {
            spdlog::debug("Sending: \n{}", msgout);
            int bytes_sent = ::send(m_socket, msgout.c_str(), msgout.length(), 0);
            if (bytes_sent < 0) {
                spdlog::error("sendto() failed. {}", GETSOCKETERRNO());
            }
            spdlog::debug("Sent {} bytes.", bytes_sent);

            return bytes_sent;
        }

std::string TCPSocket::recv() {
        int pollresponse{0};
        pollresponse = pollForRead();
        if(pollresponse == Socket::POLLRET::STIMEOUT){
            spdlog::error("No data available after {} ms", m_pConfig->socket_read_wait_ms);
            return "";
        }

        memset(&m_msgin, 0x00, sizeof(m_msgin));
        int bytes_received = ::recv(m_socket, m_msgin, sizeof(m_msgin), 0);
        if (bytes_received < 1) {
            spdlog::error("recv() failed. {}", GETSOCKETERRNO());
        }
        spdlog::debug("Received ({} bytes)", bytes_received);
        spdlog::debug("{}", m_msgin);

        return std::string(m_msgin);
}

TCPSocket::~TCPSocket(){}
        

} //namespace