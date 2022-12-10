// Copyright (c) Neil D. Harvey

#include <string>
#include <spdlog/spdlog.h>

#include "UDPSocket.h"

using namespace std;

int main()
{
    //spdlog::set_pattern();
    spdlog::set_level(spdlog::level::info);

    string msgout {"GSI\r"};
    string scanner_ip {"192.168.0.173"};
    int scanner_socket {50536};

    spdlog::info("Test starting...");
    auto myUDP = sc::UDPSocket::socket(scanner_ip, scanner_socket);

    spdlog::info("calling sendto()");
    myUDP->sendto(msgout);

    spdlog::info("calling recvfrom()");
    spdlog::info("Message Received: {}", myUDP->recvfrom());

    spdlog::info("Test ending...");

}


