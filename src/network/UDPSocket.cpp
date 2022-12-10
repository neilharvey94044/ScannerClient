// Copyright (c) Neil D. Harvey


extern "C" {
#if defined(_WIN32)
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0600
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <assert.h>
    #pragma comment(lib, "ws2_32.lib")
    #define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
    #define CLOSESOCKET(s) closesocket(s)
    #define GETSOCKETERRNO() ( WSAGetLastError() )

#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
    #include <assert.h>
    #define SOCKET int
    #define ISVALIDSOCKET(s) ((s) >= 0)
    #define CLOSESOCKET(s) close(s)
    #define GETSOCKETERRNO() (errno)

#endif

} //extern "C"

#include <memory>
#include <spdlog/spdlog.h>
#include <UDPSocket.h>

namespace sc {


// Utility function to dump data received to a file
void dump(char * fname, char * msg, int len) {
    FILE *f1 = fopen(fname, "wb");
    assert(f1);
    size_t r1 = fwrite(msg, sizeof msg[0], len, f1);
    spdlog::debug("Wrote {} elements out of {} requested\n", r1, len );
    fclose(f1);
}

// Utility function to replace control characters with spaces
void stripctrlchars(char* msg, int len) {
    spdlog::debug("Stripping control characters.");
    for(int i = 0; i < len; i++) {
        if (iscntrl(msg[i]) != 0) msg[i] = 0x20;
    }
}



class WindowsUDPSocket : public UDPSocket {
        std::string m_scanner_ip;
        const int m_scanner_port;
        char m_msgin[4096];
        SOCKET m_socket;

        struct sockaddr_in m_server_addr;
        struct sockaddr_in m_in_address;
        socklen_t m_in_address_len = sizeof(m_in_address);

        //**********temp
       const char* scanner_ip = {"192.168.0.173"};
        const int scanner_port = {50536};
        //**********

        friend std::unique_ptr<UDPSocket> UDPSocket::socket(std::string, int);

        WindowsUDPSocket(std::string scanner_ip, int scanner_port) :
            m_scanner_ip {scanner_ip},
            m_scanner_port {scanner_port}
        {
            #if defined(_WIN32)
                    WSADATA d;
                    if (WSAStartup(MAKEWORD(2, 2), &d)) {
                        spdlog::error("Failed to initialize WSA");
                    }
                #endif

                spdlog::info("Creating socket...\n");
                m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (!ISVALIDSOCKET(m_socket)) {
                    spdlog::error("Error returned from socket(). {}", GETSOCKETERRNO());
                }

        }

        int sendto(std::string msgout) override {
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

        std::string recvfrom() override {
                memset(&m_msgin, 0x00, sizeof(m_msgin));
                int bytes_received = ::recvfrom(m_socket, m_msgin, sizeof(m_msgin), 0, (struct sockaddr *) &m_in_address, &m_in_address_len);
                if (bytes_received < 1) {
                    spdlog::error("recvfrom() failed. {}", GETSOCKETERRNO());
                }
                spdlog::debug("Received ({} bytes)", bytes_received);
                stripctrlchars(m_msgin, bytes_received);

                dump("dump.bin", m_msgin, bytes_received);
                spdlog::debug("{}", m_msgin);

                return std::string(m_msgin);
        }

    public:
        virtual ~WindowsUDPSocket(){
            
            CLOSESOCKET(m_socket);

            #if defined(_WIN32)
                WSACleanup();
            #endif

        }

};

class DarwinUDPSocket : public UDPSocket {

};
class LinuxUDPSocket : public UDPSocket {

};

//static factory method
std::unique_ptr<UDPSocket> UDPSocket::socket(std::string scanner_ip, int scanner_port) {
    return std::unique_ptr<UDPSocket>(new WindowsUDPSocket(scanner_ip, scanner_port));
}

} //namespace
