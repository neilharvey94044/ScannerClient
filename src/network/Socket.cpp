// Copyright (c) Neil D. Harvey

#include <string>
#include <fstream>
#include <span>
#include <spdlog/spdlog.h>
#include <network/Socket.h>

using namespace std;

namespace sc {

// Utility function to dump data received to a file
void dump(const string fname, const string outbuf) {
    spdlog::debug("dump() dumping message to file {}", fname);
    std::fstream fs;
    fs.open(fname, std::ios::binary | std::ios::out);
    if(!fs.is_open()){
        spdlog::error("Unable to open file: {}", fname);
        return;
    }
    fs << outbuf;
    fs.close();
    
}

// Utility function to replace control characters with spaces
void stripctrlchars(span<char>& msg) {
    spdlog::debug("Stripping control characters.");
    for(char& c: msg){
        if (iscntrl(c) != 0) {c = 0x20;}

    }
}

Socket::Socket(int soctype, IPPROTO protocol ) {
                    
                #if defined(_WIN32)
                        spdlog::debug("Initializing Windows WSA");
                        WSADATA d;
                        if (WSAStartup(MAKEWORD(2, 2), &d)) {
                            spdlog::error("Failed to initialize WSA");
                        }
                #endif

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