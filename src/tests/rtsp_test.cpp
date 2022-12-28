// Copyright (c) Neil D. Harvey

#include <memory.h>
#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include <tinyxml2/tinyxml2.h>

#include "scannerclient/TCPSocket.h"
#include "scannerclient/UDPSocket.h"
#include "scannerclient/RTSPRequest.h"
#include "scannerclient/RTSPResponse.h"

#include "tests/response_test_data.h"

using namespace tinyxml2;
using namespace std;
using namespace sc;

void Test1_UDP_XML( string scanner_ip, int scanner_udp_port);  // forward
void Test2_TCP_RTSP_OPTIONS(string scanner_ip, int scanner_rtsp_port,  string user_agent, int seq);  // forward
void Test3_Verify_RTSP_Response_Parsing();  // forward
void Test4_RTSP_Pseudo_Handshake();
void Test5_RTSP_Handshake(string scanner_ip, int scanner_rtsp_port,  string user_agent, string rtp_port);  // forward



int main(int argc, char* argv[])
{

    //spdlog::set_pattern();  TODO:
    spdlog::set_level(spdlog::level::info);
    spdlog::info("Testing starting.");

    int seq = 2;

    string scanner_ip {"192.168.0.173"};
    int scanner_rtsp_port {554};
    int scanner_udp_port {50536};
    string user_agent {"ScannerClient/1.0"};
    string rtp_port {"49990-49991"};  //suggested to scanner

    // Test1_UDP_XML(scanner_ip, scanner_udp_port);
    // Test2_TCP_RTSP_OPTIONS(scanner_ip, scanner_rtsp_port, user_agent, seq);  // CAUTION:  hangs scanner
    // Test3_Verify_RTSP_Response_Parsing();
    Test4_RTSP_Pseudo_Handshake();
    // Test5_RTSP_Handshake(scanner_ip, scanner_rtsp_port, user_agent, rtp_port);

    spdlog::info("Testing ended.");
}

// Send GSI request to scanner, receive XML response, parse XML
void Test1_UDP_XML( string scanner_ip, int scanner_udp_port){
    spdlog::info("Entering Test1_UDP_XML()");

    string msgout {"GSI\r"};
    auto myUDP = make_unique<sc::UDPSocket>(scanner_ip, scanner_udp_port);

    spdlog::info("calling sendto()");
    myUDP->sendto(msgout);

    spdlog::info("calling recvfrom()");
    string udpResponse = myUDP->recvfrom();
    udpResponse.erase(0, 11);  //remove beginning of response that is not XML  TODO: make more generalized

    spdlog::info("parsing XML response");
    XMLDocument doc;
    doc.Parse(udpResponse.c_str(), udpResponse.size());

    if(doc.ErrorID()){
        spdlog::error("Tinyxml2 parse error:{} ({})", doc.ErrorName(), doc.ErrorID());
        return;

    spdlog::info("Exiting.");
}

    spdlog::debug("Printing XMLDocument");
    doc.Print();

	XMLElement* scannerInfo = doc.FirstChildElement( "ScannerInfo" );
    if(scannerInfo == nullptr){
        spdlog::error("ScannerInfo not found in XML.");
        return;
    }

    XMLElement* listElement = scannerInfo->FirstChildElement("MonitorList");
    if(listElement == nullptr){
        spdlog::error("MonitorList not found in XML.");
        return;
    }
    XMLElement* systemElement = scannerInfo->FirstChildElement("System");
    if(systemElement == nullptr){
        spdlog::error("System not found in XML.");
        return;
    }
    XMLElement* deptElement = scannerInfo->FirstChildElement("Department");
    if(deptElement == nullptr){
        spdlog::error("Department not found in XML.");
        return;
    }
    XMLElement* convElement = scannerInfo->FirstChildElement("ConvFrequency");
    if(convElement == nullptr){
        spdlog::error("ConvFrequency not found in XML.");
        return;
    }
    XMLElement* viewDescElement = scannerInfo->FirstChildElement("ViewDescription");
    if(viewDescElement == nullptr){
        spdlog::error("ViewDescription not found in XML.");
    }
    
    string listName(listElement->Attribute("Name"));

    string systemName(systemElement->Attribute("Name"));
    string deptName(deptElement->Attribute("Name"));
    string channelName(convElement->Attribute("Name"));

    if(viewDescElement->NoChildren()){
        cout << "\r\n" << "Listening..."
        << "\r\n" << "List: \t\t\t" + listName
        << "\r\n" << "System: \t\t" + systemName 
        << "\r\n" << "Department: \t\t" + deptName 
        << "\r\n" << "Channel: \t\t" + channelName 
        << endl;
    }
    else { 
        cout << "\r\n" << "Scanning..." << endl;
    }


    spdlog::info("Exiting.");
}

// Send RTSP OPTIONS to scanner
// ***Caution*** the Uniden scanner does not implement RTSP to specification, so this request
// will hang the scanner if a subsequent TEARDOWN is not sent on the same session.
void Test2_TCP_RTSP_OPTIONS(string scanner_ip, int scanner_rtsp_port,  string user_agent, int seq){
    spdlog::info("Entering Test2_TCP_RTSP_OPTIONS()");

    spdlog::info("opening socket");
    auto myTCP = make_unique<sc::TCPSocket>(scanner_ip, scanner_rtsp_port);


    spdlog::info("calling connect()");
    myTCP->connect();

    // creating OPTIONS request
    spdlog::info("creating OPTIONS request");
    auto reqOPTIONS = make_unique<RTSPRequest>(RTSPMethod::OPTIONS, scanner_ip, user_agent, seq);
    spdlog::info("\r\n"+reqOPTIONS->getString());

    spdlog::info("calling send()");
    myTCP->send(reqOPTIONS->getString());

    spdlog::info("calling recv()");
    spdlog::info("Message Received: {}", myTCP->recv()); 

    spdlog::info("Exiting");
}

// uses static test data (file: response_test_data.h) to evaluate RTSP response parsing
void Test3_Verify_RTSP_Response_Parsing(){
    spdlog::info("Entering Test3_Verify_RTSP_Response_Parsing()");
        
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
    spdlog::info("Creating RTSPResponse with OPTIONS");
    auto myRespOptions = make_unique<RTSPResponse>(RTSPMethod::OPTIONS, options_response);
    spdlog::info("Creating RTSPResponse with DESCRIBE");
    auto myRespDescribe = make_unique<RTSPResponse>(RTSPMethod::DESCRIBE, describe_response);
    spdlog::info("Creating RTSPResponse with SETUP");
    auto myRespSetup = make_unique<RTSPResponse>(RTSPMethod::SETUP, setup_response);
    spdlog::info("Creating RTSPResponse with PLAY");
    auto myRespPlay = make_unique<RTSPResponse>(RTSPMethod::PLAY, play_response);

    spdlog::info("Exiting.");
}


void Test4_RTSP_Pseudo_Handshake(){
    spdlog::info("Entering Test4_RTSP_Pseudo_Handshake()");

    int seq {2};

    string scanner_ip {"192.168.0.173"};
    int scanner_rtsp_port {554};
    int scanner_udp_port {50536};
    string user_agent {"ScannerClient/1.0"};
    string rtp_port {"49990-49991"};  //suggested to scanner


    // OPTIONS
    auto reqOPTIONS = make_unique<RTSPRequest>(RTSPMethod::OPTIONS, scanner_ip, user_agent, seq++);
    spdlog::info("\r\n"+reqOPTIONS->getString());

    RTSPResponse respOPTIONS(RTSPMethod::OPTIONS, options_response);


    // DESCRIBE
    auto reqDESCRIBE = make_unique<RTSPRequest>(RTSPMethod::DESCRIBE, scanner_ip, user_agent, seq++);
    spdlog::info("\r\n"+reqDESCRIBE->getString());

    RTSPResponse respDESCRIBE(RTSPMethod::DESCRIBE, describe_response);

    // SETUP
    auto reqSETUP = make_unique<RTSPRequest>(RTSPMethod::SETUP, scanner_ip, user_agent, seq++);
    //reqSETUP->setRTPPort(rtp_port);  //this is the requested UDP port to listen on for RTP stream
    //reqSETUP->setFullURL(respDESCRIBE.getHdrFld(RTSPHdrFld::CONTENT_BASE) + respDESCRIBE.getSDP().getSDPFld("a")) //TODO: needs more parsing for 'a'
    spdlog::info("\r\n"+reqSETUP->getString());

    RTSPResponse respSETUP(RTSPMethod::SETUP, setup_response);


    // PLAY
    auto reqPLAY = make_unique<RTSPRequest>(RTSPMethod::PLAY, scanner_ip, user_agent, seq++);
    reqPLAY->setSessionId("9999999");  //TODO: get this value from the response to SETUP
    spdlog::info("\r\n"+reqPLAY->getString());

    RTSPResponse respPLAY(RTSPMethod::PLAY, play_response);

    // TEARDOWN
    spdlog::debug("creating TEARDOWN message");
    auto reqTEARDOWN = make_unique<RTSPRequest>(RTSPMethod::TEARDOWN, scanner_ip, user_agent, seq++);
    reqTEARDOWN->setSessionId("9999999");  //TODO: get this value from the response to SETUP
    spdlog::info("\r\n"+reqTEARDOWN->getString());

    spdlog::info("Exiting.");
}


void Test5_RTSP_Handshake( string scanner_ip, int scanner_rtsp_port, string user_agent, string rtp_port){
    spdlog::info("Entering Test5_RTSP_Handshake()");

    int seq {2};

    spdlog::info("opening socket");
    TCPSocket myTCP(scanner_ip, scanner_rtsp_port);

    spdlog::info("calling connect()");
    myTCP.connect();

    // OPTIONS
    auto reqOPTIONS = make_unique<RTSPRequest>(RTSPMethod::OPTIONS, scanner_ip, user_agent, seq++);
    spdlog::info("\r\n"+reqOPTIONS->getString());
    myTCP.send(reqOPTIONS->getString());

    string respOPTIONSstr = myTCP.recv();
    RTSPResponse respOPTIONS(RTSPMethod::OPTIONS, respOPTIONSstr);


    // DESCRIBE
    auto reqDESCRIBE = make_unique<RTSPRequest>(RTSPMethod::DESCRIBE, scanner_ip, user_agent, seq++);
    spdlog::info("\r\n"+reqDESCRIBE->getString());
    myTCP.send(reqDESCRIBE->getString());

    string respDESCRIBEstr = myTCP.recv();
    RTSPResponse respDESCRIBE(RTSPMethod::DESCRIBE, respDESCRIBEstr);

    // SETUP
    auto reqSETUP = make_unique<RTSPRequest>(RTSPMethod::SETUP, scanner_ip, user_agent, seq++);
    //reqSETUP->setRTPPort(rtp_port);  //this is the requested UDP port to listen on for RTP stream
    //reqSETUP->setFullURL(respDESCRIBE.getHdrFld(RTSPHdrFld::CONTENT_BASE) + respDESCRIBE.getSDP().getSDPFld("a")) //TODO: needs more parsing for 'a'
    spdlog::info("\r\n"+reqSETUP->getString());
    myTCP.send(reqSETUP->getString());

    string respSETUPstr = myTCP.recv();
    RTSPResponse respSETUP(RTSPMethod::SETUP, respSETUPstr);


    // PLAY - send
    auto reqPLAY = make_unique<RTSPRequest>(RTSPMethod::PLAY, scanner_ip, user_agent, seq++);
    reqPLAY->setSessionId("9999999");  //TODO: get this value from the response to SETUP
    spdlog::info("\r\n"+reqPLAY->getString());
    myTCP.send(reqPLAY->getString());

    // PLAY - receive
    string respPLAYstr = myTCP.recv();
    RTSPResponse respPLAY(RTSPMethod::PLAY, respPLAYstr);

    // TEARDOWN - send
    auto reqTEARDOWN = make_unique<RTSPRequest>(RTSPMethod::TEARDOWN, scanner_ip, user_agent, seq++);
    reqTEARDOWN->setSessionId("9999999");  //TODO: get this value from the response to SETUP
    spdlog::info("\r\n"+reqTEARDOWN->getString());
    myTCP.send(reqTEARDOWN->getString());

    spdlog::info("Exiting.");
}


