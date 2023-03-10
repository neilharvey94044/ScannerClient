// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

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
    #include <poll.h>
    #define SOCKET int
    #define SOCKET_ERROR (-1)
    #define NO_ERROR (0L)
    #define ISVALIDSOCKET(s) ((s) >= 0)
    #define CLOSESOCKET(s) close(s)
    #define GETSOCKETERRNO() (errno)

#endif
}  // extern "C"


#include <string>
#include <span>
#include <memory>
#include "config/SC_CONFIG.h"

namespace sc {


class Socket {
    public:

    enum POLLRET {
        SREADY,     // ready to receive
        STIMEOUT,   // timed out - no data
        SERROR      // socket error occurred
    };

    virtual POLLRET getPollReturn();
    virtual ~Socket();
    
    protected:
        SOCKET m_socket;
        Socket(int soctype, int protocol);
        std::unique_ptr<SC_CONFIG> m_pConfig{nullptr};
        virtual int setBlocking(bool blocking);
        virtual int pollForRead();
        virtual int pollForWrite();
        POLLRET m_pollret{POLLRET::SREADY};
};

} // sc