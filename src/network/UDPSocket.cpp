// Copyright (c) Neil D. Harvey

#include <memory>
#include <span>
#include <spdlog/spdlog.h>
#include <scannerclient/UDPSocket.h>

namespace sc {

UDPSocket::UDPSocket(std::string scanner_ip, int scanner_port):
            Socket(scanner_ip, scanner_port, SOCK_DGRAM, IPPROTO_UDP)
            { }

int UDPSocket::sendto(std::string msgout){
            m_server_addr.sin_family = AF_INET;
            m_server_addr.sin_port = htons(m_scanner_port);
            inet_pton(AF_INET, m_scanner_ip.c_str(), &(m_server_addr.sin_addr));

            spdlog::debug("Sending: {}", msgout);
            int bytes_sent = ::sendto(m_socket, msgout.c_str(), msgout.length(), 0, (const struct sockaddr *) &m_server_addr, sizeof(m_server_addr));
            if (bytes_sent < 0) {
                spdlog::error("sendto() failed. {}", GETSOCKETERRNO());
            }
            spdlog::debug("Sent {} bytes.", bytes_sent);

            return bytes_sent;
        }

std::string UDPSocket::recvfrom() {
            memset(&m_msgin, 0x00, sizeof(m_msgin));
            int bytes_received = ::recvfrom(m_socket, m_msgin, sizeof(m_msgin), 0, (struct sockaddr *) &m_in_address, &m_in_address_len);
            if (bytes_received < 1) {
                spdlog::error("recvfrom() failed. {}", GETSOCKETERRNO());
            }
            spdlog::debug("Received ({} bytes)", bytes_received);
            std::span<char> buf(m_msgin, bytes_received);
            stripctrlchars(buf);

            dump("dump.bin", m_msgin);
            spdlog::debug("{}", m_msgin);

            return std::string(m_msgin);
        }

UDPSocket::~UDPSocket(){ }

} // namespace