// Copyright (c) Neil D. Harvey

#include <memory>
#include <future>
#include <iostream>
#include <string>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <tinyxml2/tinyxml2.h>
#include <rtaudio/RtAudio.h>


#include "network/TCPSocket.h"
#include "network/UDPSocket.h"
#include "network/RTSPRequest.h"
#include "network/RTSPResponse.h"
#include "network/RTSPSession.h"
#include "network/RTPSession.h"
#include "audio/ISC_Audio.h"
#include "audio/SC_RTaudio.h"
#include "tests/response_test_data.h"
#include "status/ScannerStatus.h"

using namespace tinyxml2;
using namespace std;
using namespace sc;

void Test1_UDP_XML( string scanner_ip, int scanner_udp_port);  // Send GSI command to scanner and parse response
void Test2_TCP_RTSP_OPTIONS(string scanner_ip, int scanner_rtsp_port,  string user_agent, int seq);  // forward
void Test3_Verify_RTSP_Response_Parsing();  // forward
void Test4_RTSP_Pseudo_Handshake();
void Test5_RTSP_Handshake(string scanner_ip, int scanner_rtsp_port,  string user_agent, string rtp_port);  // forward
void Test6_Play_Audio_From_Scanner();
void Test7_Play_Audio_From_Scanner();  //encapsulated in a SC_RTPaudio class
void Test8_UDP_Push_Scanner_Info(string scanner_ip, int scanner_udp_port);  // Send PSI command to scanner and parse response
void Test9_UDP_Push_Scanner_Info_Threaded();  // Send PSI command to scanner and parse response using thread
void Test10_UDP_Non_Blocking();  // Test non-blocking UDP
void Test11_Sound_And_Status(); // combine sound and status
void Test12_MSM_Reset(string scanner_ip, int scanner_udp_port); // send MSM command to reset scanner
void Test13_MDL_Get_Model(string scanner_ip, int scanner_udp_port); // send MDL command to get model
void Test14_Socket_Connect_Timeout_WIN_Specific(string scanner_ip, int scanner_rtsp_port);  //reduce connect time when scanner is offline
void Test15_Simulate_Keys(string scanner_ip, int scanner_udp_port);  //send various commands to the scanner
void Test16_Update_Date_Time(string scanner_ip, int scanner_udp_port); 






int main(int argc, char* argv[])
{

    //spdlog::set_pattern();  TODO:
    spdlog::set_level(spdlog::level::debug);
    //spdlog::set_pattern("[%H:%M:%S] [%L] [%#  %s] [thread %t] %v");
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^[%l]%$ [%t] %v");
    spdlog::info("Testing starting.");

    int seq = 2;

    string scanner_ip {"192.168.0.173"};
    int scanner_rtsp_port {554};
    int scanner_udp_port {50536};
    string user_agent {"ScannerClient/1.0"};
    string hint_rtp_port {"49990-49991"};  //suggested to scanner

    // Test1_UDP_XML(scanner_ip, scanner_udp_port);
    // Test2_TCP_RTSP_OPTIONS(scanner_ip, scanner_rtsp_port, user_agent, seq);  // CAUTION:  hangs scanner
    // Test3_Verify_RTSP_Response_Parsing();
    // Test4_RTSP_Pseudo_Handshake();
    // Test5_RTSP_Handshake(scanner_ip, scanner_rtsp_port, user_agent, hint_rtp_port);
    // Test6_Play_Audio_From_Scanner();
    // Test7_Play_Audio_From_Scanner();
    // Test8_UDP_Push_Scanner_Info(scanner_ip, scanner_udp_port);
    // Test9_UDP_Push_Scanner_Info_Threaded();  // Send PSI command to scanner and parse response using thread
    // Test10_UDP_Non_Blocking();  // Test non-blocking UDP
    // Test11_Sound_And_Status();
    // Test12_MSM_Reset(scanner_ip, scanner_udp_port);
    // Test13_MDL_Get_Model(scanner_ip, scanner_udp_port); // send MDL command to get model
    // Test14_Socket_Connect_Timeout_WIN_Specific(scanner_ip, scanner_rtsp_port);
    // Test15_Simulate_Keys(scanner_ip, scanner_udp_port);  //send various commands to the scanner
    Test16_Update_Date_Time(scanner_ip, scanner_udp_port);  //update scanners clock

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

    // remove extraneous control characters
    // TODO: use an algorithm and get rid of stripctrlchars()
    span buf(udpResponse);
    stripctrlchars(buf);

    udpResponse.erase(0, 11);  //remove beginning of response that is not XML  TODO: make more generalized

    spdlog::info("parsing XML response");
    XMLDocument doc;
    doc.Parse(udpResponse.c_str(), udpResponse.size());

    if(doc.ErrorID()){
        spdlog::error("Tinyxml2 parse error:{} ({})", doc.ErrorName(), doc.ErrorID());
        return;
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
    int actual_rtp_port{0};
    string user_agent {"ScannerClient/1.0"};
    string hint_rtp_port {"49990-49991"};  //suggested to scanner
    string current_sessionID {""};
    string audio_channel{""};


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
    reqSETUP->setHintRTPPort(hint_rtp_port);  //this is the requested UDP port to listen on for RTP stream
    audio_channel = respDESCRIBE.getAudioChannel();
    reqSETUP->setAudioChannel(audio_channel);
    spdlog::info("\r\n"+reqSETUP->getString());

    RTSPResponse respSETUP(RTSPMethod::SETUP, setup_response);
    current_sessionID = respSETUP.getSession();
    actual_rtp_port = respSETUP.getRTPPort();
    spdlog::info("RTP Port for stream is:{}", actual_rtp_port); //"client_port" attribute from "Transport" header


    // PLAY
    auto reqPLAY = make_unique<RTSPRequest>(RTSPMethod::PLAY, scanner_ip, user_agent, seq++);
    reqPLAY->setSessionId(current_sessionID); 
    spdlog::info("\r\n"+reqPLAY->getString());

    RTSPResponse respPLAY(RTSPMethod::PLAY, play_response);

    // TEARDOWN
    spdlog::debug("creating TEARDOWN message");
    auto reqTEARDOWN = make_unique<RTSPRequest>(RTSPMethod::TEARDOWN, scanner_ip, user_agent, seq++);
    reqTEARDOWN->setSessionId(current_sessionID);
    spdlog::info("\r\n"+reqTEARDOWN->getString());

    spdlog::info("Exiting.");
}

// Actual RTSP handshake with the scanner
// OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN fully implemented
void Test5_RTSP_Handshake( string scanner_ip, int scanner_rtsp_port, string user_agent, string hint_rtp_port){
    spdlog::info("Entering Test5_RTSP_Handshake()");

    int seq {2};

    int actual_rtp_port{0};
    string current_sessionID {""};
    string audio_channel{""};

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
    spdlog::info("OPTIONS: {}", respOPTIONS.getStatus());


    // DESCRIBE
    auto reqDESCRIBE = make_unique<RTSPRequest>(RTSPMethod::DESCRIBE, scanner_ip, user_agent, seq++);
    spdlog::info("\r\n"+reqDESCRIBE->getString());
    myTCP.send(reqDESCRIBE->getString());

    string respDESCRIBEstr = myTCP.recv();
    RTSPResponse respDESCRIBE(RTSPMethod::DESCRIBE, respDESCRIBEstr);
    spdlog::info("DESCRIBE: {}", respDESCRIBE.getStatus());

    // SETUP
    auto reqSETUP = make_unique<RTSPRequest>(RTSPMethod::SETUP, scanner_ip, user_agent, seq++);
    reqSETUP->setHintRTPPort(hint_rtp_port);  //this is the requested UDP port to listen on for RTP stream
    audio_channel = respDESCRIBE.getAudioChannel();
    reqSETUP->setAudioChannel(audio_channel);
    spdlog::info("\r\n"+reqSETUP->getString());
    myTCP.send(reqSETUP->getString());

    string respSETUPstr = myTCP.recv();
    RTSPResponse respSETUP(RTSPMethod::SETUP, respSETUPstr);
    current_sessionID = respSETUP.getSession();
    actual_rtp_port = respSETUP.getRTPPort();
    spdlog::info("SETUP: {}", respSETUP.getStatus());
    spdlog::info("RTP Port for stream is:{}", actual_rtp_port); //"client_port" attribute from "Transport" header


    // PLAY - send
    auto reqPLAY = make_unique<RTSPRequest>(RTSPMethod::PLAY, scanner_ip, user_agent, seq++);
    reqPLAY->setSessionId(current_sessionID); 
    spdlog::info("\r\n"+reqPLAY->getString());
    myTCP.send(reqPLAY->getString());

    // PLAY - receive
    string respPLAYstr = myTCP.recv();
    RTSPResponse respPLAY(RTSPMethod::PLAY, respPLAYstr);
    spdlog::info("PLAY: {}", respPLAY.getStatus());
/* 
    if(respPLAY.getStatus() == 200){
        // receive PCMU audio over RTP - for now fixed number of datagrams written to a file
        RTPSession rtp_session(scanner_ip, actual_rtp_port);
        rtp_session.run();
    } */

    // TEARDOWN - send
    auto reqTEARDOWN = make_unique<RTSPRequest>(RTSPMethod::TEARDOWN, scanner_ip, user_agent, seq++);
    reqTEARDOWN->setSessionId(current_sessionID);
    spdlog::info("\r\n"+reqTEARDOWN->getString());
    myTCP.send(reqTEARDOWN->getString());

    spdlog::info("Exiting.");
}


// RTAudio Callback Function
int callback( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{
    spdlog::debug("In callback");
    // The buffer comes in as a pointer to a shared_ptr, we want the actual shared_ptr
    std::shared_ptr<AudioBuffer> audio_buf_ptr =  (*(std::shared_ptr<AudioBuffer>*) data);

    //TODO: optimize this; inefficient because getAudio() was designed for a container
    rtpbuf buf;
    audio_buf_ptr->getAudio(buf);
    unsigned short* outbuf_ptr = (unsigned short*) outputBuffer;
    for(unsigned short s: buf){
        *outbuf_ptr++ = s;
    }

    if(audio_buf_ptr->getStopped()){
        spdlog::debug("callback stopped");
        return 2;
    }

  return 0;
}


void Test6_Play_Audio_From_Scanner(){

    RtAudio dac;
    unsigned int channels=1, sample_rate=8000, bufferFrames=320, offset = 0, device = 0;
    if ( dac.getDeviceCount() < 1 ) {
        spdlog::error("\nNo audio devices found! Check your RtAudio build script.\n");
        exit( 0 );
    }
    device = dac.getDefaultOutputDevice();
    spdlog::debug("Audio output device:{}", device);

    RtAudio::StreamParameters oParams;
    oParams.deviceId = device;
    oParams.nChannels = channels;
    oParams.firstChannel = offset;

    try {
        shared_ptr<RTSPSession> rtsp_session = make_shared<RTSPSession>();
        shared_ptr<AudioBuffer> audio_buffer_ptr = rtsp_session->getAudioBuffer();
        std::promise<bool> rtsp_success_promise;
        std::future<bool> rtsp_success_future = rtsp_success_promise.get_future();
        rtsp_session->start(std::move(rtsp_success_promise));
        if(audio_buffer_ptr != nullptr && rtsp_success_future.get()){
            dac.openStream( &oParams, NULL, RTAUDIO_SINT16, sample_rate, &bufferFrames, &callback, (void *)&audio_buffer_ptr );
            spdlog::debug("Opened Audio Stream");
            dac.startStream();
            spdlog::debug("Started Audio Stream");

            spdlog::debug("Stopped RTSPSession");
        }
        int stop;
        cin >> stop;

        rtsp_session->stop();

    }
    catch ( RtAudioError& e ) {
        spdlog::error("RtAudio Error: {}", e.getMessage());
  }

}

void Test7_Play_Audio_From_Scanner(){
    auto audio = make_unique<SC_RTaudio>();

    // will use this for GUI support to allow picking device
    auto devices = audio->getAudioDevices();
    for(auto d: devices){
        spdlog::info("Device ID:[{}] Name:[{}] Channels:[{}] Default:[{}]", 
        std::get<0>(d), 
        std::get<1>(d), 
        std::get<2>(d), 
        std::get<3>(d) );

        if(std::get<3>(d)){ // pick the default device for this test
            audio->setAudioDevice(std::get<0>(d));
        }
    }


    audio->start();

    spdlog::debug("Exited audio->start()");

    // pause here waiting for keyboard input of an integer
    int stop;
    cin >> stop;
    spdlog::debug("Exited cin pause");

    audio->stop();

    spdlog::debug("After audio->stop()");

}


void Test8_UDP_Push_Scanner_Info(std::string scanner_ip, int scanner_udp_port){
    spdlog::info("Entering Test8_UDP_Push_Scanner_Info()");

    string msgout {"PSI,200\r"};
    auto myUDP = make_unique<sc::UDPSocket>(scanner_ip, scanner_udp_port);

    spdlog::info("calling sendto()");
    myUDP->sendto(msgout);

    spdlog::info("Initial ok response");
    string udpResponse = myUDP->recvfrom();
    spdlog::debug("PSI Response:{}", udpResponse);
    if(!udpResponse.find("OK")){
        spdlog::error("Did not receive OK from PSI Request");
        return;
    }

    for(int i=0; i < 2000; i++){
        spdlog::info("calling recvfrom()");
        string udpResponse = myUDP->recvfrom();
 
         // remove extraneous control characters
        // TODO: use an algorithm and get rid of stripctrlchars()
        span buf(udpResponse);
        stripctrlchars(buf);

        //spdlog::info("PSI Response:\n{}", udpResponse);

        udpResponse.erase(0, udpResponse.find("<?xml"));  //remove beginning of response that is not XML

        spdlog::info("parsing XML response");
        XMLDocument doc;
        doc.Parse(udpResponse.c_str(), udpResponse.size());

        if(doc.ErrorID()){
            spdlog::error("Tinyxml2 parse error:{} ({})", doc.ErrorName(), doc.ErrorID());
            //return;
        }
        // get top level element
        XMLElement* scannerInfo = doc.FirstChildElement( "ScannerInfo" );
        if(scannerInfo == nullptr){
            spdlog::error("ScannerInfo not found in XML.");
            continue;
            //return;
        }
        // get View Description
        std::string overWriteText{};
        XMLElement *viewDescription = nullptr, *overWrite = nullptr;
        viewDescription = scannerInfo->FirstChildElement( "ViewDescription" );
        if(viewDescription != nullptr){
            overWrite = viewDescription->FirstChildElement("OverWrite");
            if(overWrite != nullptr){
                overWriteText = overWrite->Attribute("Text");
            }
        }


        // If we don't get "ViewDescription/OverWrite/Text" the scanner has an active signal
        //if(overWriteText.empty()){
            XMLPrinter printer;
            doc.Print( &printer );
            cout << printer.CStr() << endl << endl;
       // }
       // else cout << "Scanning..." << endl;

    }


}

void Test9_UDP_Push_Scanner_Info_Threaded(){
    spdlog::info("Starting Test9_UDP_Push_Scanner_Info_Threaded");

    ScannerStatus ss;

    ss.start([] (shared_ptr<SC_STATUS> cs){

        spdlog::info("Channel:{}  Playing:{}", cs->ConvFreq_Name, cs->Playing);
    }
    );

    int stop{0};
    cin >> stop;

    ss.stop();
}

void Test10_UDP_Non_Blocking(){
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

void Test11_Sound_And_Status(){

    spdlog::info("Starting Audio");
   auto audio = make_unique<SC_RTaudio>();

    // will use this for GUI support to allow picking device
    auto devices = audio->getAudioDevices();
    for(auto d: devices){
        spdlog::info("Device ID:[{}] Name:[{}] Channels:[{}] Default:[{}]", 
        std::get<0>(d), 
        std::get<1>(d), 
        std::get<2>(d), 
        std::get<3>(d) );

        if(std::get<3>(d)){ // pick the default device for this test
            audio->setAudioDevice(std::get<0>(d));
        }
    }


    audio->start();


    ScannerStatus ss;

    spdlog::info("Starting Status");
    ss.start([] (shared_ptr<SC_STATUS> cs){
/*         if(cs->Playing){
            cout    << "============================================" << endl
            << "Mode \t\t"                  << cs->Mode << endl
            << "V_Screen \t\t"              << cs->VScreen << endl
            << "MonitorListName \t\t"       << cs->MonitorListName << endl
            << "SystemName \t\t"            << cs->SystemName << endl
            << "DepartmentName \t\t"        << cs->DepartmentName << endl
            << "ConvFrequencyName \t\t"     << cs->ConvFrequencyName << endl
            << "ConvFrequencySvcType \t\t"  << cs->ConvFrequencySvcType << endl
            << "ConvFrequencyMod \t\t"      << cs->ConvFrequencyMod << endl
            << "PropSig \t\t"               << cs->PropSig << endl
            << "PropP25Status \t\t"         << cs->PropP25Status << endl
            << "TS_SiteName \t\t"           << cs->TS_SiteName << endl
            << "TS_SiteFreq \t\t"           << cs->TS_SiteFreq << endl
            << "TS_TGID \t\t"               << cs->TS_TGID << endl
            << "OverWriteText \t\t"         << cs->OverWriteText << endl
            << "Playing \t\t"               << cs->Playing << endl;
        }
        else{ 
            cout << cs->OverWriteText << endl;
        } */

        spdlog::info("System:{} Dept:{} Channel:{}  Playing:{}", cs->System_Name, cs->Dept_Name, cs->ConvFreq_Name, cs->Playing);
    }
    );

    int stop{0};
    cin >> stop;

    spdlog::info("Stopping Status and Audio");
    ss.stop();    
    audio->stop();
}

void Test12_MSM_Reset(string scanner_ip, int scanner_udp_port){
   spdlog::info("Entering Test12_MSM_Reset()");

    string msgout {"MSM,1\r"};
    auto myUDP = make_unique<sc::UDPSocket>(scanner_ip, scanner_udp_port);

    spdlog::info("calling sendto()");
    myUDP->sendto(msgout);

    spdlog::info("calling recvfrom()");
    string udpResponse = myUDP->recvfrom();
    cout << udpResponse << endl;

}

void Test13_MDL_Get_Model(string scanner_ip, int scanner_udp_port){
   spdlog::info("Entering Test13_MDL_Get_Model()");

    string msgout {"MDL\r"};
    auto myUDP = make_unique<sc::UDPSocket>(scanner_ip, scanner_udp_port);

    spdlog::info("calling sendto()");
    myUDP->sendto(msgout);

    spdlog::info("calling recvfrom()");
    string udpResponse = myUDP->recvfrom();
    cout << udpResponse << endl;

}

// Attempt to connect to IP address without blocking
void Test14_Socket_Connect_Timeout_WIN_Specific(string scanner_ip, int scanner_rtsp_port){
    spdlog::info("Entering Test14_Socket_Connect_Timeout_WIN_Specific");

    struct sockaddr_in m_server_addr;
    struct sockaddr_in m_in_address;
    int iResult{0};
    socklen_t m_in_address_len = sizeof(m_in_address);

    #if defined(_WIN32)
            spdlog::debug("Initializing Windows WSA");
            WSADATA d;
            if (WSAStartup(MAKEWORD(2, 2), &d)) {
                spdlog::error("Failed to initialize WSA");
                return;
            }
    #endif

    SOCKET m_socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!ISVALIDSOCKET(m_socket)) {
        spdlog::error("Error returned from socket(). {}", GETSOCKETERRNO());
        return;
    }

    //TODO: this is Windows only code - need a Linux substitute to disable blocking.
    unsigned long non_blocking = -1;
    iResult = ioctlsocket(m_socket, FIONBIO, &non_blocking);
    if (iResult != NO_ERROR){
        printf("ioctlsocket change to non-blocking failed with error: %ld\n", iResult);
        return;
    }

    spdlog::debug("Connecting");
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(scanner_rtsp_port);
    inet_pton(AF_INET, scanner_ip.c_str(), &(m_server_addr.sin_addr));
    iResult = ::connect(m_socket, (const struct sockaddr *) &m_server_addr, sizeof(m_server_addr));
    if((iResult == SOCKET_ERROR) && (GETSOCKETERRNO() != WSAEWOULDBLOCK)){
        spdlog::error("connect() failed. {}", GETSOCKETERRNO());
        return;
    }

    // use poll to wait a short time for the connection to occur
    spdlog::debug("Invoking WSAPoll");
    int ret{0};
    WSAPOLLFD fdarray;
    fdarray.fd = m_socket;
    fdarray.events = POLLWRNORM;
    ret = WSAPoll(&fdarray, 1, 1000);
    if(ret == SOCKET_ERROR){
        spdlog::error("Socket error on WSAPoll {}", GETSOCKETERRNO());
        return;
    }
    if(ret && (fdarray.revents & POLLWRNORM)){  // ready to write - does that mean connected?
         spdlog::debug("Socket is ready to write.");
    }
    else{
        // this state is reached when the scanner is off or already connected to software for sound
        spdlog::error("Socket is not ready to write rdarray.revents:{}", fdarray.revents);
    }


    unsigned long blocking = 0;
    iResult = ioctlsocket(m_socket, FIONBIO, &blocking);
    if (iResult != NO_ERROR){
        printf("ioctlsocket change to blocking failed with error: %ld\n", iResult);
        return;
    }

    spdlog::debug("Closing socket");
    CLOSESOCKET(m_socket);

    #if defined(_WIN32)
        WSACleanup();
    #endif
    spdlog::info("Exiting Test14_Socket_Connect_Timeout_WIN_Specific");

}


void Test15_Simulate_Keys(string scanner_ip, int scanner_udp_port){
  spdlog::info("Entering Test15_Simulate_Keys()");

    auto myUDP = make_unique<sc::UDPSocket>(scanner_ip, scanner_udp_port);

    // "KEY,[key],[mode]"
    // mode: P - press, L - Long press, H - press and hold until release received, R - release hold

    // System
    string  system_hold {"KEY,A,P\r"};
    myUDP->sendto(system_hold);

    // Department
    //string dept_hold {"KEY,B,P\r"};
    //myUDP->sendto(dept_hold);

    // Channel
    string  channel_hold {"KEY,C,P\r"};
    //myUDP->sendto(channel_hold);

    // Rotary - right aka "bump"
    //string rotaryright {"KEY,>,P\r"};
    //myUDP->sendto(rotaryright);

    // Number - 0 followed by [enter]
    //string number_one {"KEY,0,P\rKEY,E,P\r"};
    //myUDP->sendto(number_one);

    // Priority Key
    //string priority {"KEY,F,P\rKEY,.,P\r"};
    //myUDP->sendto(priority);

    // Close Call?
    //string closecall {"KEY,"}

    // Weather Scan- put in channel hold mode then [func][wx] key...soft key 1 to resume scan
/*     myUDP->sendto(channel);
    string weather_scan {"KEY,F,P\rKEY,6,H\r"};
    myUDP->sendto(weather_scan);
    std::this_thread::sleep_for(3s);
    string weather_release {"KEY,6,R\r"};
    myUDP->sendto(weather_release); */

    // Weather Priority - put in channel hold mode then push wx key
    //myUDP->sendto(channel_hold);
    //string weather_priority {"KEY,F,P\rKEY,6,P\r"};
    //myUDP->sendto(weather_priority);
}

void Test16_Update_Date_Time(string scanner_ip, int scanner_udp_port){

    // DTM,[DayLightSaving],[YYYY],[MM],[DD],[hh],[mm],[ss][\r]

    auto myUDP = make_unique<sc::UDPSocket>(scanner_ip, scanner_udp_port);

    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string s(30, '\0');
    std::strftime(&s[0], s.size(), "DTM,1,%Y,%m,%d,%H,%M,%S\r", std::localtime(&now));
    cout << s << endl;
    myUDP->sendto(s);
    cout << myUDP->recvfrom() << endl;
}
