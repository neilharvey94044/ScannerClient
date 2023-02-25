// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <string>
#include <fstream>
#include <span>
#include <spdlog/spdlog.h>
#include "utils/utils.h"
#include "network/Socket.h"


using namespace std;

namespace sc {

Socket::Socket(int soctype, int protocol ) :
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
    iReturn = WSAPoll(&fdarray, 1, m_pConfig->socket_write_wait_ms;
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
#else

int Socket::pollForRead(){
    int iReturn{0};
    struct pollfd fds[1];
    fds[0].fd = m_socket;
    fds[0].revents = POLLIN;
    iReturn = poll(fds, 1, m_pConfig->socket_read_wait_ms);
    if(iReturn == SOCKET_ERROR){
        spdlog::error("Socket error on POLLIN {}", GETSOCKETERRNO());
        return (m_pollret = Socket::POLLRET::SERROR);
    }
    if(iReturn && (fds[0].events & POLLIN)){
        spdlog::debug("Socket is ready to read");
        return (m_pollret = Socket::POLLRET::SREADY);
    }
    else{
        spdlog::error("Socket is not ready to read fds.revents:{}", fds[0].revents);
        return (m_pollret = Socket::POLLRET::STIMEOUT);
    }
}

int Socket::pollForWrite(){
    int iReturn{0};
    struct pollfd fds[1];
    fds[0].fd = m_socket;
    fds[0].revents = POLLOUT;
    iReturn = poll(fds, 1, m_pConfig->socket_write_wait_ms);
    if(iReturn == SOCKET_ERROR){
        spdlog::error("Socket error on poll() {}", GETSOCKETERRNO());
        return (m_pollret = Socket::POLLRET::SERROR);
    }
    if(iReturn && (fds[0].events & POLLOUT)) {  // read to write
        spdlog::debug("Socket is ready to write");
        return (m_pollret = Socket::POLLRET::SREADY);
    }
    else{
        // this state is read when the scanner is off or already connected
        spdlog::error("Socket is not ready to write fds.revents:{}", fds[0].revents);
        return (m_pollret = Socket::POLLRET::STIMEOUT);
    }

}
#endif


int Socket::setBlocking(bool blocking){
    int iResult{0};
    #if defined(_WIN32)
        unsigned long desired_state = blocking ? 0 : -1;
        iResult = ioctlsocket(m_socket, FIONBIO, &desired_state);
        if (iResult != NO_ERROR){
        spdlog::error("ioctlsocket change to non-blocking failed with error [{}]", GETSOCKETERRNO());
        return iResult;
    }
    #else
        int flags = fcntl(m_socket, F_GETFL, 0);  // get existing flags
        if (flags == SOCKET_ERROR){
            spdlog::error("Error obtaining socket flags");
            return SOCKET_ERROR;
        }
        flags = (blocking) ?  (flags & O_NONBLOCK) : (flags & ~O_NONBLOCK);
        iResult = fcntl(m_socket, F_SETFL, flags);
        if(iResult != NO_ERROR){
            spdlog::error("Error changing socket to non-blocking [{}]", GETSOCKETERRNO());
            return iResult;
        }
    #endif
    return iResult;
}


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