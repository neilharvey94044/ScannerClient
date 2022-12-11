// Copyright (c) Neil D. Harvey

#include <string>
#include <spdlog/spdlog.h>
#include <network/Socket.h>

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


Socket::Socket(std::string scanner_ip, int scanner_port, int soctype, IPPROTO protocol ):
                m_scanner_ip {scanner_ip},
                m_scanner_port {scanner_port}{
                    
                #if defined(_WIN32)
                        spdlog::info("Initializing Windows WSA");
                        WSADATA d;
                        if (WSAStartup(MAKEWORD(2, 2), &d)) {
                            spdlog::error("Failed to initialize WSA");
                        }
                #endif

                spdlog::info("Creating socket...\n");
                m_socket = ::socket(PF_INET, soctype, protocol);
                if (!ISVALIDSOCKET(m_socket)) {
                    spdlog::error("Error returned from socket(). {}", GETSOCKETERRNO());
                }
        }

Socket::~Socket(){
            spdlog::debug("Closing socket");
            CLOSESOCKET(m_socket);

            #if defined(_WIN32)
                WSACleanup();
            #endif
        }

    
} //namespace