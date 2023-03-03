// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <thread>
#include <chrono>
#include "utils/utils.h"
#include "network/UDPSocket.h"
#include "control/SControl.h"

namespace sc{

SControl::SControl() :
            m_pConfig{SC_CONFIG::get()}
            {}

void SControl::send_system_hold(){
    sendToScanner(msg.system_hold);
}

void SControl::send_dept_hold(){
    sendToScanner(msg.dept_hold);
}

void SControl::send_channel_hold(){
    sendToScanner(msg.channel_hold);
}

void SControl::send_bump(){
    sendToScanner(msg.bump);
}

void SControl::send_reboot(){
    sendToScanner(msg.reboot);
}

void SControl::send_weather_scan(){
    auto myUDP = make_unique<sc::UDPSocket>(m_pConfig->ip_address, m_pConfig->status_udp_port);
    myUDP->sendto(msg.channel_hold);
    myUDP->sendto(msg.weather_scan);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(3s);
    myUDP->sendto(msg.weather_release);
}

void SControl::updateClock(){
    // DTM,[DayLightSaving],[YYYY],[MM],[DD],[hh],[mm],[ss][\r]
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string s(30, '\0');
    std::strftime(&s[0], s.size(), "DTM,1,%Y,%m,%d,%H,%M,%S\r", std::localtime(&now));
    sendToScanner(s);
}

void SControl::send_avoid(){
    sendToScanner(msg.avoid);
}

void SControl::sendToScanner(std::string msg){
    auto myUDP = make_unique<sc::UDPSocket>(m_pConfig->ip_address, m_pConfig->status_udp_port);
    myUDP->sendto(msg);
}

std::string SControl::getModel() {
    auto myUDP = make_unique<sc::UDPSocket>(m_pConfig->ip_address, m_pConfig->status_udp_port);
    myUDP->sendto(msg.get_model);
    string model = myUDP->recvfrom();
    stripctrlchars(model); 
    model = (model.find("MDL") != std::string::npos) ? model.substr(model.find_first_of(",")+1) : "?";
    return model;
}

std::string SControl::getFirmware(){
    auto myUDP = make_unique<sc::UDPSocket>(m_pConfig->ip_address, m_pConfig->status_udp_port);
    myUDP->sendto(msg.get_firmware);
    string firmware = myUDP->recvfrom();
    stripctrlchars(firmware);
    firmware = (firmware.find("VER") != std::string::npos) ? firmware.substr(firmware.find_first_of(",")+1) : "?";
    return firmware;
}

} // namespace