// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include <string>
#include <memory>
#include "config/SC_CONFIG.h"

namespace sc {

class SControl {


    public:
        SControl();
        void send_system_hold();
        void send_dept_hold();
        void send_channel_hold();
        void send_bump();
        void send_reboot();
        void send_weather_scan();
        void send_avoid();
        void updateClock();
        std::string getModel();
        std::string getFirmware();

    private:
        void sendToScanner(std::string msg);
        std::unique_ptr<SC_CONFIG> m_pConfig{nullptr};

        struct {
            std::string system_hold {"KEY,A,P\r"};
            std::string dept_hold {"KEY,B,P\r"};
            std::string channel_hold {"KEY,C,P\r"};
            std::string bump {"KEY,>,P\r"}; // rotary right
            std::string reboot {"MSM,1\r"};
            std::string avoid {"KEY,L,P\r"};
            std::string weather_scan {"KEY,F,P\rKEY,6,H\r"};
            std::string weather_release {"KEY,6,R\r"};
            std::string get_model{"MDL\r"};
            std::string get_firmware{"VER\r"};


        } msg;

};

} //namespace