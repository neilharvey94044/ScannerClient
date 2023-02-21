// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include <memory>
#include <thread>
#include <string>
#include <atomic>
#include <tinyxml2/tinyxml2.h>
#include "network/UDPSocket.h"

namespace sc{

struct SC_STATUS {
    std::string Mode{};
    std::string V_Screen{};
    std::string MonitorListName{};
    std::string SystemName{};
    std::string SystemHold{};
    std::string DepartmentName{};

    std::string MonitorList_Name{};
    std::string MonitorList_Index{};
    std::string MonitorList_ListType{};
    std::string MonitorList_Q_Key{};
    std::string MonitorList_N_Tag{};
    std::string MonitorList_DB_Counter{};

    std::string System_Name{};
    std::string System_Index{};
    std::string System_Avoid{};
    std::string System_SystemType{};
    std::string System_Q_Key{};
    std::string System_N_Tag{};
    std::string System_Hold{};

    std::string Dept_Name{};
    std::string Dept_Index{};
    std::string Dept_Avoid{};
    std::string Dept_Q_Key{};
    std::string Dept_Hold{};

    std::string ConvFreq_Name{};
    std::string ConvFreq_Index{};
    std::string ConvFreq_Avoid{};
    std::string ConvFreq_Freq{};
    std::string ConvFreq_Mod{};
    std::string ConvFreq_N_Tag{};
    std::string ConvFreq_Hold{};
    std::string ConvFreq_SvcType{};
    std::string ConvFreq_P_Ch{};
    std::string ConvFreq_SAS{};
    std::string ConvFreq_SAD{};
    std::string ConvFreq_LVL{};
    std::string ConvFreq_IFX{};

    std::string Site_Name{};
    std::string Site_Index{};
    std::string Site_Avoid{};
    std::string Site_Q_Key{};
    std::string Site_Hold{};
    std::string Site_Mod{};

    std::string SiteFreq_Freq{};
    std::string SiteFreq_SAS{};
    std::string SiteFreq_IFX{};

    std::string TGID_Name{};
    std::string TGID_Index{};
    std::string TGID_Avoid{};
    std::string TGID_TGID{};
    std::string TGID_N_Tag{};
    std::string TGID_Hold{};
    std::string TGID_SvcType{};
    std::string TGID_P_Ch{};
    std::string TGID_LVL{};

    std::string UnitID_Name{};

    std::string wx_Mode{};
    std::string wx_CH_No{};
    std::string wx_Freq{};
    std::string wx_Mod{};
    std::string wx_Hold{};

    std::string Prop_F{};
    std::string Prop_VOL{};
    std::string Prop_SQL{};
    std::string Prop_Sig{};
    std::string Prop_Battery{};
    std::string Prop_Att{};
    std::string Prop_Rec{};
    std::string Prop_KeyLock{};
    std::string Prop_P25Status{};
    std::string Prop_Mute{};
    std::string Prop_A_Led{};
    std::string Prop_Dir{};
    std::string Prop_Rssi{};


    std::string OverWriteText{};
    bool Playing{false};

} ;


class ScannerStatus {
    public:
        ScannerStatus();
        void start( std::function<void(std::shared_ptr<SC_STATUS> cs)> );
        void stop();
        bool isStarted() const;

    private:
        void execute( std::function<void(std::shared_ptr<SC_STATUS> cs)> );
        std::string getAttribute(tinyxml2::XMLElement* element, std::string attrName);

        std::unique_ptr<SC_CONFIG> m_pConfig;
        std::shared_ptr<SC_STATUS> produceStatus(tinyxml2::XMLDocument& doc);
        std::string m_period{"400"};  // Scanner will push status information every m_period milliseconds
        std::atomic_bool m_started{false};
        std::atomic_bool m_running{false};
        std::unique_ptr<std::thread> m_status_thread{nullptr};


};

} // namespace