// Copyright (c) Neil D. Harvey

#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include <network/TCPSocket.h>

namespace sc {

TCPSocket::TCPSocket(std::string scanner_ip, int scanner_port) :
            Socket(scanner_ip, scanner_port, SOCK_STREAM, IPPROTO_TCP)
        { }

int TCPSocket::connect() {
            spdlog::debug("Connecting");
            m_server_addr.sin_family = AF_INET;
            m_server_addr.sin_port = htons(m_scanner_port);
            inet_pton(AF_INET, m_scanner_ip.c_str(), &(m_server_addr.sin_addr));
            int sockerr = ::connect(m_socket, (const struct sockaddr *) &m_server_addr, sizeof(m_server_addr));
            if (sockerr < 0) {
                spdlog::error("connect() failed. {}", GETSOCKETERRNO());
            }
            return sockerr;
        }

int TCPSocket::send(std::string msgout) {
            spdlog::debug("Sending: {}", msgout);
            int bytes_sent = ::send(m_socket, msgout.c_str(), msgout.length(), 0);
            if (bytes_sent < 0) {
                spdlog::error("sendto() failed. {}", GETSOCKETERRNO());
            }
            spdlog::debug("Sent {} bytes.", bytes_sent);

            return bytes_sent;
        }

std::string TCPSocket::recv() {
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