// Copyright (c) Neil D. Harvey

#pragma once

#include <memory>
#include <thread>
#include <string>
#include <atomic>
#include <tinyxml2/tinyxml2.h>
#include "scannerclient/sc.h"
#include "scannerclient/UDPSocket.h"

namespace sc{

struct SC_STATUS {
    std::string Mode{};
    std::string VScreen{};
    std::string MonitorListName{};
    std::string SystemName{};
    std::string DepartmentName{};
    std::string ConvFrequencyName{};
    std::string ConvFrequencySvcType{};
    std::string ConvFrequencyFreq{};
    std::string ConvFrequencyMod{};

    std::string TS_SiteName{};
    std::string TS_SiteFreq{};
    std::string TS_TGID{};
    std::string TS_TGIDNAME{};

    std::string PropSig{};
    std::string PropP25Status{};


    std::string OverWriteText{};
    bool Playing{false};

} ;


class ScannerStatus {
    public:
        ScannerStatus():
            m_server_ip {SC_CONFIG.ip_address},
            m_server_port{SC_CONFIG.status_udp_port},
            m_udp_socket{m_server_ip, m_server_port}
            {};

        void start( void (*outfunc)(std::shared_ptr<SC_STATUS> cs) );
        void stop();

    private:
        void execute( void (*outfunc)(std::shared_ptr<SC_STATUS> cs) );
        std::shared_ptr<SC_STATUS> produceStatus(tinyxml2::XMLDocument& doc);
        std::string m_period{"400"};  // Scanner will push status information every m_period milliseconds
        std::string m_server_ip;
        int m_server_port;
        UDPSocket m_udp_socket;
        std::atomic_bool m_running{false};
        std::unique_ptr<std::thread> m_status_thread;


};

} // namespace