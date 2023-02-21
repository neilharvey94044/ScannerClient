// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <tinyxml2/tinyxml2.h>
#include "status/ScannerStatus.h"

using namespace std;
using namespace tinyxml2;


namespace sc {

ScannerStatus::ScannerStatus() :
                m_pConfig{SC_CONFIG::get()}
                {}

void ScannerStatus::start( std::function<void(std::shared_ptr<SC_STATUS> cs)> outfunc){
    if(m_started){
        spdlog::error("Attempt to start an active ScannerStatus. Invoke stop() first.");
        return;
    }
    m_started = true;
    m_status_thread = std::make_unique<std::thread>(&ScannerStatus::execute, this, outfunc );
}

void ScannerStatus::execute( std::function<void(std::shared_ptr<SC_STATUS> cs)> outfunc){
    spdlog::debug("Entering ScannerStatus::execute()");
    UDPSocket udp_socket(m_pConfig->ip_address, m_pConfig->status_udp_port);

    // when scanner stops sending status messages come here to start over
    restart:

    m_running = true;

    string msgout = fmt::format("PSI,{}\r", m_period);

    spdlog::debug("calling sendto()");
    udp_socket.sendto(msgout);

    string udpResponse = udp_socket.recvfrom();
        
    spdlog::debug("PSI Response:{}", udpResponse);
    if(udpResponse.find("OK") == string::npos && udpResponse.find("PSI,<XML>") == string::npos){
        spdlog::error("Did not receive OK or valid XML from PSI Request");
        return;
    }
    XMLDocument doc;

    while(m_running){

        doc.Clear();

        spdlog::debug("Calling recvfrom()");
        string udpResponse = udp_socket.recvfrom();
        if(udp_socket.getPollReturn() == Socket::POLLRET::STIMEOUT){
            spdlog::debug("ScannerStatus resending PSI command");
            goto restart;       // keeps the current thread and restarts PSI
        }

 
         // remove extraneous control characters
        // TODO: use an algorithm and get rid of stripctrlchars()
        spdlog::debug("Stripping control characters");
        span buf(udpResponse);
        stripctrlchars(buf);

        spdlog::debug("PSI Response:{}", udpResponse);

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

    // get top level element
    XMLElement* scannerInfo = doc.FirstChildElement( "ScannerInfo" );
    if(scannerInfo == nullptr){
        spdlog::error("ScannerInfo not found in XML.");
        return nullptr;
    }

    // Get ScannerInfo Attributes
    cs->Mode = getAttribute(scannerInfo, "Mode");
    cs->V_Screen = getAttribute(scannerInfo, "V_Screen");


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

    // Mode = *_scan (includes conventional, trunk, custom_with, and cchits_with)
    if(cs->V_Screen.find("_scan")){

        // Get System
        spdlog::debug("Getting System");
        XMLElement* systemElement = scannerInfo->FirstChildElement("System");
        if(systemElement != nullptr){

            cs->System_Name        = getAttribute(systemElement, "Name");
            cs->System_Index       = getAttribute(systemElement, "Index");
            cs->System_Avoid       = getAttribute(systemElement, "Avoid");
            cs->System_SystemType  = getAttribute(systemElement, "SystemType");
            cs->System_Q_Key       = getAttribute(systemElement, "Q_Key");
            cs->System_N_Tag       = getAttribute(systemElement, "N_Tag");
            cs->System_Hold        = getAttribute(systemElement, "Hold");


        }

        // Get Department
        spdlog::debug("Getting Department");
        XMLElement* deptElement = scannerInfo->FirstChildElement("Department");
        if(deptElement != nullptr){
            cs->Dept_Name        = getAttribute(deptElement, "Name");
            cs->Dept_Index       = getAttribute(deptElement, "Index");
            cs->Dept_Avoid       = getAttribute(deptElement, "Avoid");
            cs->Dept_Q_Key       = getAttribute(deptElement, "Q_Key");
            cs->Dept_Hold        = getAttribute(deptElement, "Hold");
        }

        // Get Monitor List
        spdlog::debug("Getting MonitorList");
        XMLElement* listElement = scannerInfo->FirstChildElement("MonitorList");
        if(listElement != nullptr){
            cs->MonitorList_Name        = getAttribute(listElement, "Name");
            cs->MonitorList_Index       = getAttribute(listElement, "Index");
            cs->MonitorList_ListType    = getAttribute(listElement, "ListType");
            cs->MonitorList_Q_Key       = getAttribute(listElement, "Q_Key");
            cs->MonitorList_N_Tag       = getAttribute(listElement, "N_Tag");
            cs->MonitorList_DB_Counter  = getAttribute(listElement, "DB_Counter");
        }

    }

    if(cs->V_Screen == "conventional_scan"){
        // Get Conventional Channel Name, Frequency, Service Type, Modulation
        spdlog::debug("Getting ConvFrequency");
        XMLElement* convElement = scannerInfo->FirstChildElement("ConvFrequency");
        if(convElement != nullptr){
            cs->ConvFreq_Name     = getAttribute(convElement, "Name");
            cs->ConvFreq_Index    = getAttribute(convElement, "Index");
            cs->ConvFreq_Avoid    = getAttribute(convElement, "Avoid");
            cs->ConvFreq_Freq     = getAttribute(convElement, "Freq");
            cs->ConvFreq_Mod      = getAttribute(convElement, "Mod");
            cs->ConvFreq_N_Tag    = getAttribute(convElement, "N_Tag");
            cs->ConvFreq_Hold     = getAttribute(convElement, "Hold");
            cs->ConvFreq_SvcType  = getAttribute(convElement, "SvcType");
            cs->ConvFreq_P_Ch     = getAttribute(convElement, "P_Ch");
            cs->ConvFreq_SAS      = getAttribute(convElement, "SAS");
            cs->ConvFreq_SAD      = getAttribute(convElement, "SAD");
            cs->ConvFreq_LVL      = getAttribute(convElement, "LVL");
            cs->ConvFreq_IFX      = getAttribute(convElement, "IFX");
        }
    }

    if(cs->V_Screen == "trunk_scan"){
        spdlog::debug("Getting Trunk Scan Attributes");
        XMLElement *siteElement = scannerInfo->FirstChildElement("Site");
        if(siteElement != nullptr){
            cs->Site_Name    = getAttribute(siteElement, "Name");
            cs->Site_Index   = getAttribute(siteElement, "Index");
            cs->Site_Avoid   = getAttribute(siteElement, "Avoid");
            cs->Site_Q_Key   = getAttribute(siteElement, "Q_Key");
            cs->Site_Hold    = getAttribute(siteElement, "Hold");
            cs->Site_Mod     = getAttribute(siteElement, "Mod");


        }
        XMLElement *siteFrequency = scannerInfo->FirstChildElement("SiteFrequency");
        if(siteFrequency != nullptr){
            cs->SiteFreq_Freq = getAttribute(siteFrequency, "Freq");
            cs->SiteFreq_IFX = getAttribute(siteFrequency, "IFX");
            cs->SiteFreq_SAS = getAttribute(siteFrequency, "SAS");
        }
        XMLElement *tgidElement = scannerInfo->FirstChildElement("TGID");
        if(tgidElement != nullptr){
            cs->TGID_Name      = getAttribute(tgidElement, "Name");
            cs->TGID_Index     = getAttribute(tgidElement, "Index");
            cs->TGID_Avoid     = getAttribute(tgidElement, "Avoid");
            cs->TGID_TGID      = getAttribute(tgidElement, "TGID");
            cs->TGID_N_Tag     = getAttribute(tgidElement, "N_Tag");
            cs->TGID_Hold      = getAttribute(tgidElement, "Hold");
            cs->TGID_SvcType   = getAttribute(tgidElement, "SvcType");
            cs->TGID_P_Ch      = getAttribute(tgidElement, "P_Ch");
            cs->TGID_LVL       = getAttribute(tgidElement, "LVL");

        }
        XMLElement *unitIDElement = scannerInfo->FirstChildElement("UnitID");
        if(unitIDElement != nullptr){
            cs->UnitID_Name    = getAttribute(unitIDElement, "Name" );
        }

    }
    // Get Weather items
    if(cs->V_Screen == "wx_alert"){
        spdlog::debug("Getting Weather Attributes");
        XMLElement *wxModeElement = scannerInfo->FirstChildElement("WxMode");
        if(wxModeElement != nullptr){
            cs->wx_Mode    = getAttribute(wxModeElement, "Mode");
        }
        XMLElement *wxChannelElement = scannerInfo->FirstChildElement("WxChannel");
        if(wxChannelElement != nullptr){
            cs->wx_CH_No   = getAttribute(wxChannelElement, "CH_No");
            cs->wx_Freq    = getAttribute(wxChannelElement, "Freq");
            cs->wx_Mod     = getAttribute(wxChannelElement, "Mod");
            cs->wx_Hold    = getAttribute(wxChannelElement, "Hold");
        }
    }



    // Get Properties
    spdlog::debug("Getting Property Items");
    XMLElement* propElement = scannerInfo->FirstChildElement("Property");
    if(propElement != nullptr){
        cs->Prop_F          = getAttribute(propElement, "F");
        cs->Prop_VOL        = getAttribute(propElement, "VOL");
        cs->Prop_SQL        = getAttribute(propElement, "SQL");
        cs->Prop_Sig        = getAttribute(propElement, "Sig");
        cs->Prop_Battery    = getAttribute(propElement, "Battery");
        cs->Prop_Att        = getAttribute(propElement, "Att");
        cs->Prop_Rec        = getAttribute(propElement, "Rec");
        cs->Prop_KeyLock    = getAttribute(propElement, "KeyLock");
        cs->Prop_P25Status  = getAttribute(propElement, "P25Status");
        cs->Prop_Mute       = getAttribute(propElement, "Mute");
        cs->Prop_A_Led      = getAttribute(propElement, "A_Led");
        cs->Prop_Dir        = getAttribute(propElement, "Dir");
        cs->Prop_Rssi       = getAttribute(propElement, "Rssi");

    }

    spdlog::debug("produceStatus returning");
    return std::move(cs);

}

std::string ScannerStatus::getAttribute(XMLElement *element, std::string attrName){
    if(element == nullptr) return "";
    const XMLAttribute *attr = element->FindAttribute(attrName.c_str());
    return (attr != nullptr) ? attr->Value() : "";
}

bool ScannerStatus::isStarted() const{
    return m_started;
}


void ScannerStatus::stop(){
    spdlog::debug("ScannerStatus stopping");
    m_started = false;
    m_running = false;
    if(m_status_thread != nullptr && m_status_thread->joinable()){
        m_status_thread->join();
    }
    spdlog::debug("Exiting ScannerStatus::stop");
}

} // namespace