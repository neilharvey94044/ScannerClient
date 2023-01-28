// Copyright (c) Neil D. Harvey

#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <tinyxml2/tinyxml2.h>
#include "status/ScannerStatus.h"

using namespace std;
using namespace tinyxml2;


namespace sc {

void ScannerStatus::start( void (*outfunc)(shared_ptr<SC_STATUS> cs) ){
    m_status_thread = std::make_unique<std::thread>(&ScannerStatus::execute, this, outfunc );
}

void ScannerStatus::execute( void (*outfunc)(shared_ptr<SC_STATUS> cs) ){
    spdlog::debug("Entering ScannerStatus::execute()");
    m_running = true;

    string msgout = fmt::format("PSI,{}\r", m_period);

    spdlog::debug("calling sendto()");
    m_udp_socket.sendto(msgout);

    string udpResponse = m_udp_socket.recvfrom();
    spdlog::debug("PSI Response:{}", udpResponse);
    if(!udpResponse.find("OK")){
        spdlog::error("Did not receive OK from PSI Request");
        return;
    }
    XMLDocument doc;

    while(m_running){

        doc.Clear();

        spdlog::debug("Calling recvfrom()");
        string udpResponse = m_udp_socket.recvfrom();
 
         // remove extraneous control characters
        // TODO: use an algorithm and get rid of stripctrlchars()
        spdlog::debug("Stripping control characters");
        span buf(udpResponse);
        stripctrlchars(buf);

        //spdlog::debug("PSI Response:{}", udpResponse);

        //TODO: optimize the erase
        udpResponse.erase(0, udpResponse.find("<?xml"));  //remove beginning of response that is not XML

        spdlog::debug("parsing XML response");
        doc.Parse(udpResponse.c_str(), udpResponse.size());

        if(doc.Error()){
            spdlog::error("Tinyxml2 parse error:{} ({})", doc.ErrorName(), doc.ErrorID());
            continue;
        }

        auto cs = produceStatus(doc);

        spdlog::debug("About to invoke outfunc()");
        if(cs != nullptr){
            outfunc(cs);
        }
        spdlog::debug("After outfunc()");

    }

}

shared_ptr<SC_STATUS> ScannerStatus::produceStatus(XMLDocument& doc){
    spdlog::debug("Entering ScannerStatus::produceStatus");

    auto cs = make_shared<SC_STATUS>(); // current status

    // for now output the entire XML
  /*   XMLPrinter printer;
    doc.Print( &printer );
    spdlog::info(printer.CStr()); */

    // get top level element
    XMLElement* scannerInfo = doc.FirstChildElement( "ScannerInfo" );
    if(scannerInfo == nullptr){
        spdlog::error("ScannerInfo not found in XML.");
        return nullptr;
    }

    // Get ScannerInfo Attributes
    const XMLAttribute *mode = scannerInfo->FindAttribute("Mode");
    if(mode != nullptr) cs->Mode = mode->Value();
    const XMLAttribute *vscreen = scannerInfo->FindAttribute("V_Screen");
    if(vscreen != nullptr) cs->VScreen = vscreen->Value();

    // get View Description
    spdlog::debug("Getting ViewDescription");
    std::string overWriteText{};
    XMLElement *viewDescription = nullptr, *overWrite = nullptr;
    viewDescription = scannerInfo->FirstChildElement( "ViewDescription" );
    if(viewDescription != nullptr){
        overWrite = viewDescription->FirstChildElement("OverWrite");
        if(overWrite != nullptr){
            overWriteText = overWrite->Attribute("Text");
        }
    }

    if(overWriteText.empty()){
        cs->Playing = true;  // scanner is on a channel
    }
    else{
        cs->OverWriteText = overWriteText;
    }

    // Get Monitor List Name
    spdlog::debug("Getting MonitorList");
    XMLElement* listElement = scannerInfo->FirstChildElement("MonitorList");
    if(listElement != nullptr){
        const XMLAttribute *listname =  listElement->FindAttribute("Name");
        if(listname != nullptr)  cs->MonitorListName = listname->Value();
    }

    // Get System Name
    spdlog::debug("Getting System");
    XMLElement* systemElement = scannerInfo->FirstChildElement("System");
    if(systemElement != nullptr){
        const XMLAttribute *systemName = systemElement->FindAttribute("Name");
        if(systemName != nullptr) cs->SystemName = systemName->Value();
    }

    // Get Department Name
    spdlog::debug("Getting Department");
    XMLElement* deptElement = scannerInfo->FirstChildElement("Department");
    if(deptElement != nullptr){
        const XMLAttribute *deptName = deptElement->FindAttribute("Name");
        if(deptName != nullptr) cs->DepartmentName = deptName->Value();
    }

    // Get Conventional Channel Name, Frequency, Service Type, Modulation
    spdlog::debug("Getting ConvFrequency Items");
    XMLElement* convElement = scannerInfo->FirstChildElement("ConvFrequency");
    if(convElement != nullptr){
        const XMLAttribute *freqName = convElement->FindAttribute("Name");
        if(freqName != nullptr) cs->ConvFrequencyName = freqName->Value();

        const XMLAttribute *svcType = convElement->FindAttribute("SvcType");
        if(svcType != nullptr) cs->ConvFrequencySvcType = svcType->Value();

        const XMLAttribute *freq = convElement->FindAttribute("Freq");
        if(freq != nullptr) cs->ConvFrequencyFreq = freq->Value();

        const XMLAttribute *mod = convElement->FindAttribute("Mod");
        if(mod != nullptr) cs->ConvFrequencyMod = mod->Value();

    }

    // Get Trunk Scanning
    spdlog::debug("Getting Trunk Values");
    XMLElement *siteElement = scannerInfo->FirstChildElement("Site");
    if(siteElement != nullptr){
        const XMLAttribute *siteName = siteElement->FindAttribute("Name");
        if(siteName != nullptr) cs->TS_SiteName = siteName->Value();
    }
    XMLElement *siteFrequency = scannerInfo->FirstChildElement("SiteFrequency");
    if(siteFrequency != nullptr){
        const XMLAttribute *freq = siteFrequency->FindAttribute("Freq");
        if(freq != nullptr) cs->TS_SiteFreq = freq->Value();
    }
    XMLElement *tgidElement = scannerInfo->FirstChildElement("TGID");
    if(tgidElement != nullptr){
        const XMLAttribute *tgid = tgidElement->FindAttribute("TGID");
        if(tgid != nullptr) cs->TS_TGID = tgid->Value();
        const XMLAttribute *tgidName = tgidElement->FindAttribute("Name");
        if(tgidName != nullptr) cs->TS_TGIDNAME = tgidName->Value();
    }

    // Get Properties
    spdlog::debug("Getting Property Items");
    XMLElement* propElement = scannerInfo->FirstChildElement("Property");
    if(propElement != nullptr){
        const XMLAttribute *signal = propElement->FindAttribute("Sig");
        if(signal != nullptr) cs->PropSig = signal->Value();

        const XMLAttribute *p25 = propElement->FindAttribute("P25Status");
        if(p25 != nullptr) cs->PropP25Status = p25->Value();
    }

    spdlog::debug("produceStatus returning");
    return std::move(cs);

}

void ScannerStatus::stop(){
    spdlog::debug("ScannerStatus stopping");
    m_running = false;
    m_status_thread->join();
}

} // namespace