// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+


#pragma once

#include <memory>
#include <vector>
#include <array>
#include "wx/wx.h"
#include "wx/app.h"
#include "wx/confbase.h"

#include "control/SControl.h"
#include "config/SC_CONFIG.h"
#include "status/ScannerStatus.h"
#include "audio/SC_RTaudio.h"
#include "gui/SettingsDialog.h"

class SCFrame : public wxFrame
{


public:
    SCFrame(const wxString& title);

    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    // these are currently invoked from SCApp so made public
    // Making SCApp a friend of SCFrame blew out the MSFT compiler stack
    //TODO: Enqueue a message?
    void bump(wxCommandEvent& WXUNUSED(event));
    void channelHold(wxCommandEvent& WXUNUSED(event));

private:
    void systemHold(wxCommandEvent& WXUNUSED(event));
    void deptHold(wxCommandEvent& WXUNUSED(event));
    void reboot(wxCommandEvent& WXUNUSED(event));
    void weatherScan(wxCommandEvent& WXUNUSED(event));
    void avoid(wxCommandEvent& WXUNUSED(event));
    void updateClock(wxCommandEvent& WXUNUSED(event));

    void startScanner(wxCommandEvent& e);
    void stopScanner(wxCommandEvent& e);
    void editSettings(wxCommandEvent& e);
    void updateStatus();

    const wxColor m_red{wxColor(182, 50, 43)};
    const wxColor m_orange{wxColor(230, 84, 39)};
    const wxColor m_yellow{wxColor(230, 191, 49)};
    const wxColor m_black{wxColor(0, 0, 0)};

    wxPanel *m_status_panel{};
    std::array< wxStaticText*, 6 > m_mid_status_text{};
    std::array< wxStaticText*, 5 > m_bot_left_status_text{};
    std::array< wxStaticText*, 5 > m_bot_right_status_text{};
    SettingsData m_settingsData;

    std::unique_ptr<sc::SControl> m_control;
    std::unique_ptr<sc::SC_CONFIG> m_pConfig;
    std::unique_ptr<sc::SC_RTaudio> m_audio;
    std::unique_ptr<sc::ScannerStatus> m_ss;

    wxDECLARE_EVENT_TABLE();
};


