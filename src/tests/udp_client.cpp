// Copyright (c) Neil D. Harvey

#include <string>
#include <memory>
#include <spdlog/spdlog.h>

#include <network/UDPSocket.h>

using namespace std;

int main()
{
    //spdlog::set_pattern();
    spdlog::set_level(spdlog::level::info);

    string msgout {"GSI\r"};
    string scanner_ip {"192.168.0.173"};
    int scanner_socket {50536};

    spdlog::info("Test starting...");
    auto myUDP = make_unique<sc::UDPSocket>(scanner_ip, scanner_socket);

    spdlog::info("calling sendto()");
    myUDP->sendto(msgout);

    spdlog::info("calling recvfrom()");
    spdlog::info("Message Received: {}", myUDP->recvfrom());

    spdlog::info("Test ending...");

}


