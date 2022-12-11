// Copyright (c) Neil D. Harvey

#include <string>
#include <spdlog/spdlog.h>

#include <network/TCPSocket.h>

using namespace std;

int main()
{
    //spdlog::set_pattern();
    spdlog::set_level(spdlog::level::debug);

    string optionsOut {"OPTIONS rtsp://192.168.0.173:554/au:scanner.au RTSP/1.0\r\nCSeq: 1\r\nUser-Agent: ScannerClient/1.0\r\n\r\n"};
    string scanner_ip {"192.168.0.173"};
    int scanner_socket {554};

    spdlog::info("Test starting...");
    auto myTCP = make_unique<sc::TCPSocket>(scanner_ip, scanner_socket);

    spdlog::info("calling connect()");
    myTCP->connect();

    spdlog::info("calling send()");
    myTCP->send(optionsOut);

    spdlog::info("calling recv()");
    spdlog::info("Message Received: {}", myTCP->recv());

    spdlog::info("Test ending...");

}
