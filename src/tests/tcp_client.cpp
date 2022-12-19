// Copyright (c) Neil D. Harvey

#include <memory.h>
#include <string>
#include <spdlog/spdlog.h>

#include <network/TCPSocket.h>
#include <network/RTSPRequest.h>
#include <network/RTSPResponse.h>

#include <network/response_test_data.h>

using namespace std;
using namespace sc;

int main(int argc, char* argv[])
{
    //spdlog::set_pattern();
    spdlog::set_level(spdlog::level::debug);
    int seq = 2;
    string sessionID {"66622000"}; //TODO: get value from SETUP response


    string scanner_ip {"192.168.0.173"};
    int scanner_port {554};
    string user_agent {"ScannerClient/1.0"};
    string rtp_port {"49990-49991"};

    spdlog::info("Test starting...");


// Using response message tests
spdlog::info("OPTIONS RESPONSE:");
spdlog::info(options_response);
spdlog::info("DESCRIBE RESPONSE:");
spdlog::info(describe_response);
spdlog::info("SETUP RESPONSE:");
spdlog::info(setup_response);
spdlog::info("PLAY RESPONSE:");
spdlog::info(play_response);

// Response message tests
//spdlog::info("Creating RTSPResponse with OPTIONS");
//auto myRespOptions = make_unique<RTSPResponse>(RTSPMethod::OPTIONS, options_response);
spdlog::info("Creating RTSPResponse with DESCRIBE");
auto myRespDescribe = make_unique<RTSPResponse>(RTSPMethod::DESCRIBE, describe_response);
//spdlog::info("Creating RTSPResponse with SETUP");
//auto myRespSetup = make_unique<RTSPResponse>(RTSPMethod::SETUP, setup_response);
//spdlog::info("Creating RTSPResponse with PLAY");
//auto myRespPlay = make_unique<RTSPResponse>(RTSPMethod::PLAY, play_response);



/* Request message tests
    // OPTIONS
    auto reqOPTIONS = make_unique<RTSPRequest>(RTSPMethod::OPTIONS, scanner_ip, user_agent, seq++);
    spdlog::info(reqOPTIONS->getString());

    // DESCRIBE
    auto reqDESCRIBE = make_unique<RTSPRequest>(RTSPMethod::DESCRIBE, scanner_ip, user_agent, seq++);
    spdlog::info(reqDESCRIBE->getString());

    // SETUP
    auto reqSETUP = make_unique<RTSPRequest>(RTSPMethod::SETUP, scanner_ip, user_agent, seq++);
    reqSETUP->setRTPPort(rtp_port);  //this is the requested UDP port to listen on for RTP stream
    reqSETUP->setFullURL(respDESCRIBE.getHdrFld(RTSPHdrFld::CONTENT_BASE) + respDESCRIBE.getSDP().getSDPFld("a")) //TODO: needs more parsing for 'a'
    spdlog::info(reqSETUP->getString());

    // PlAY
    auto reqPLAY = make_unique<RTSPRequest>(RTSPMethod::PLAY, scanner_ip, user_agent, seq++);
    reqPLAY->setSessionId(sessionID);  //TODO: get this value from the response to SETUP
    spdlog::info(reqPLAY->getString());

    // TEARDOWN
    auto reqTEARDOWN = make_unique<RTSPRequest>(RTSPMethod::TEARDOWN, scanner_ip, user_agent, seq++);
    reqTEARDOWN->setSessionId(sessionID);  //TODO: get this value from the response to SETUP
    spdlog::info(reqTEARDOWN->getString());
 */





/* Socket tests

    auto myTCP = make_unique<sc::TCPSocket>(scanner_ip, scanner_socket);

    spdlog::info("calling connect()");
    myTCP->connect();

    spdlog::info("calling send()");
    //myTCP->send(optionsOut);

    spdlog::info("calling recv()");
    spdlog::info("Message Received: {}", myTCP->recv()); */

    spdlog::info("Test ending...");

}
