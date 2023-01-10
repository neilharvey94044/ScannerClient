// Copyright (c) Neil D. Harvey

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
    #define SOCKET int
    #define ISVALIDSOCKET(s) ((s) >= 0)
    #define CLOSESOCKET(s) close(s)
    #define GETSOCKETERRNO() (errno)

#endif
}  // extern "C"


#include <string>
#include <span>
#include <memory>

namespace sc {

void dump(const std::string fname, const std::string outbuf);
void stripctrlchars(std::span<char>& msg);

class Socket {
    public:
        virtual ~Socket();
    
    //TODO: make these member variables private
    protected:
        SOCKET m_socket;
        Socket(int soctype, IPPROTO protocol);
};

} // sc