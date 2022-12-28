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

using namespace std;

namespace sc {

void dump(const string fname, const string outbuf);
void stripctrlchars(span<char>& msg);

class Socket {
    public:
        virtual ~Socket();
    
    protected:
        SOCKET m_socket;
        std::string m_scanner_ip;
        const int m_scanner_port;
        Socket(std::string scanner_ip, int scanner_port, int soctype, IPPROTO protocol);
};

} // sc