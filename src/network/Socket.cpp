// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

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

Socket::Socket(int soctype, IPPROTO protocol ) :
                m_pConfig{SC_CONFIG::get()}
{
                    
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

//TODO: implement a Linux/OSI version of poll() or morph this one to work for all platforms
#if defined(_WIN32)
int Socket::pollForRead() {
    int ret{0};
    WSAPOLLFD fdarray;
    fdarray.fd = m_socket;
    fdarray.events = POLLRDNORM;
    ret = WSAPoll(&fdarray, 1, m_pConfig->socket_read_wait_ms);
    if(ret && (fdarray.revents & POLLRDNORM)){  // ready to receive
         return (m_pollret = Socket::POLLRET::SREADY);
    }
    if(ret == SOCKET_ERROR){
        spdlog::error("Socket error on WSAPoll {}", GETSOCKETERRNO());
        return (m_pollret = Socket::POLLRET::SERROR);
    }
    return (m_pollret = Socket::POLLRET::STIMEOUT);  // timed out before data arrived

}

int Socket::pollForWrite() {
    int iReturn{0};
    WSAPOLLFD fdarray;
    fdarray.fd = m_socket;
    fdarray.events = POLLWRNORM;
    iReturn = WSAPoll(&fdarray, 1, 1000);
    if(iReturn == SOCKET_ERROR){
        spdlog::error("Socket error on WSAPoll {}", GETSOCKETERRNO());
        return (m_pollret = Socket::POLLRET::SERROR);
    }
    if(iReturn && (fdarray.revents & POLLWRNORM)){  // ready to write
         spdlog::debug("Socket is ready to write.");
         return (m_pollret = Socket::POLLRET::SREADY);
    }
    else{
        // this state is reached when the scanner is off or already connected
        spdlog::error("Socket is not ready to write rdarray.revents:{}", fdarray.revents);
        return (m_pollret = Socket::POLLRET::STIMEOUT);
    }
}
#endif

Socket::POLLRET Socket::getPollReturn(){
    return m_pollret;
}

Socket::~Socket(){
            spdlog::debug("Closing socket");
            CLOSESOCKET(m_socket);

            #if defined(_WIN32)
                WSACleanup();
            #endif
        }




} //namespace