// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <memory>
#include <vector>
#include <array>
#include <spdlog/spdlog.h>
//#include <wx/fileconf.h>

#include "utils/utils.h"
#include "control/SControl.h"
#include "config/SC_CONFIG.h"
#include "gui/SettingsDialog.h"
#include "gui/SCFrame.h"
#include "gui/scApp.h"



// IDs for the controls and the menu commands
enum
{
    SC_Quit = wxID_EXIT,
    SC_About = wxID_ABOUT,
    SC_Start = wxID_LAST + 1,  // following add 1 automatically
    SC_Stop,
    SC_Settings,
    SC_System_Hold,
    SC_Dept_Hold,
    SC_Channel_Hold,
    SC_Bump,
    SC_Reboot,
    SC_Weather_Scan,
    SC_Avoid,
    SC_Update_Clock
};

wxBEGIN_EVENT_TABLE(SCFrame, wxFrame)
    EVT_MENU(SC_Quit,  SCFrame::OnQuit)
    EVT_MENU(SC_About, SCFrame::OnAbout)
    EVT_BUTTON(SC_Start, SCFrame::startScanner)
    EVT_BUTTON(SC_Stop, SCFrame::stopScanner)
    EVT_BUTTON(SC_Settings, SCFrame::editSettings)
    EVT_BUTTON(SC_System_Hold, SCFrame::systemHold)
    EVT_BUTTON(SC_Dept_Hold, SCFrame::deptHold)
    EVT_BUTTON(SC_Channel_Hold, SCFrame::channelHold)
    EVT_BUTTON(SC_Bump, SCFrame::bump)
    EVT_BUTTON(SC_Reboot, SCFrame::reboot)
    EVT_BUTTON(SC_Weather_Scan, SCFrame::weatherScan)
    EVT_BUTTON(SC_Avoid, SCFrame::avoid)
    EVT_BUTTON(SC_Update_Clock, SCFrame::updateClock)
    EVT_CLOSE(SCFrame::OnClose)
wxEND_EVENT_TABLE()




SCFrame::SCFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title),
        m_pConfig{sc::SC_CONFIG::get()},
        m_control{std::make_unique<sc::SControl>()},
        m_audio(std::make_unique<sc::SC_RTaudio>()),
        m_ss(std::make_unique<sc::ScannerStatus>())

{
    // set the frame icon
    #if defined(_WIN32)
        SetIcon(wxICON(sample));
    #endif
 
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // About item
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(SC_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(SC_Quit, "E&xit\tAlt-X", "Quit this program");

    // append menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // attach menu bar to the frame
    //SetMenuBar(menuBar);



/*
    // configuration file - created where the executable is invoked
    wxConfigBase *pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, "sc.cfg", wxEmptyString, wxCONFIG_USE_RELATIVE_PATH);
    pConfig->SetRecordDefaults(false);
    wxConfigBase::Set(pConfig);
*/

    // create a status bar
    CreateStatusBar(2);
    std::string model = m_control->getModel();
    std::string firmware = m_control->getFirmware();
    std::string ip = !m_pConfig->ip_address.empty() ? m_pConfig->ip_address : "?";
    std::string statusbarstring =  fmt::format("[ {}][ {}][ {} ]", model, firmware, m_pConfig->ip_address);
    SetStatusText(statusbarstring);

 // Status Panel
    m_status_panel  = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize); //wxSize(800, 300)
    m_status_panel->SetBackgroundColour(m_black);
    wxBoxSizer *status_sizer = new wxBoxSizer(wxVERTICAL);

    // The Main Status e.g. System, Department, Channel, etc.
    for(int i=0; i < 6; i++){
        m_mid_status_text.at(i) = new wxStaticText(m_status_panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
        status_sizer->Add(m_mid_status_text.at(i), wxSizerFlags(0).Border(wxLEFT, 10));
    }
    // Make System, Department, and Channel a large font and match colors to scanner
    wxFont large_font = m_mid_status_text[0]->GetFont().Scale(2);
    //large_font.Scale(2);
    m_mid_status_text[0]->SetFont(large_font);  // System
    m_mid_status_text[2]->SetFont(large_font);  // Department
    m_mid_status_text[4]->SetFont(large_font);  // Channel
    m_mid_status_text[0]->SetOwnForegroundColour(m_red);
    m_mid_status_text[1]->SetOwnForegroundColour(m_red);
    m_mid_status_text[2]->SetOwnForegroundColour(m_orange);
    m_mid_status_text[3]->SetOwnForegroundColour(m_orange);
    m_mid_status_text[4]->SetOwnForegroundColour(m_yellow);
    m_mid_status_text[5]->SetOwnForegroundColour(m_yellow);
    m_mid_status_text[0]->SetLabelText("                                                                ");  //64 characters maximum

   
// sizers and text for the bottom of status screen
    auto status_sizer_bot_H = new wxStaticBoxSizer(wxHORIZONTAL, m_status_panel);
    auto status_sizer_bot_left_V = new wxBoxSizer(wxVERTICAL);
    auto status_sizer_bot_right_V = new wxBoxSizer(wxVERTICAL);

    for(int i=0; i < 5; i++){
        m_bot_left_status_text.at(i) = new wxStaticText(m_status_panel, wxID_ANY, "                    ", wxDefaultPosition, wxDefaultSize);
        m_bot_left_status_text.at(i)->SetOwnForegroundColour(wxColor(167, 117, 153));
        status_sizer_bot_left_V->Add(m_bot_left_status_text.at(i), wxSizerFlags(0).Left());

        m_bot_right_status_text.at(i) = new wxStaticText(m_status_panel, wxID_ANY, "                    ", wxDefaultPosition, wxDefaultSize);
        m_bot_right_status_text.at(i)->SetOwnForegroundColour(wxColor(167, 117, 153));
        status_sizer_bot_right_V->Add(m_bot_right_status_text.at(i), wxSizerFlags(0).Left());
    }
    status_sizer_bot_H->Add(status_sizer_bot_left_V, wxSizerFlags(1).Border(wxLEFT, 10));
    status_sizer_bot_H->AddSpacer(300);  //TODO: there must be a better way, tried wxEXPAND/wxSHAPED for a filler box sizer...too big
    

    status_sizer_bot_H->Add(status_sizer_bot_right_V, wxSizerFlags(1).Border(wxRIGHT, 10));
    status_sizer_bot_H->AddStretchSpacer(1);

    status_sizer->Add(status_sizer_bot_H, wxSizerFlags(0).Border(wxLEFT, 10));

    // System/Department/Channel Buttons below status
    wxBoxSizer *status_sizer_soft_buttons = new wxBoxSizer(wxHORIZONTAL);
    status_sizer_soft_buttons->Add(new wxButton(m_status_panel, SC_System_Hold, "System"), wxSizerFlags().Expand().Border(wxALL, 10));
    status_sizer_soft_buttons->Add(new wxButton(m_status_panel, SC_Dept_Hold, "Department"), wxSizerFlags().Expand().Border(wxALL, 10));
    status_sizer_soft_buttons->Add(new wxButton(m_status_panel, SC_Channel_Hold, "Channel"), wxSizerFlags().Expand().Border(wxALL, 10));
    status_sizer->Add(status_sizer_soft_buttons, wxSizerFlags(1).Border(wxLEFT, 10));

    m_status_panel->SetSizerAndFit(status_sizer);


    // Control Panel
    wxPanel *control_panel  = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize); // wxSize(300, 300)
    control_panel->SetBackgroundColour(wxColor(16, 18, 22));
    wxBoxSizer *control_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *control_line1_sizer = new wxBoxSizer(wxHORIZONTAL);
    control_line1_sizer->Add(new wxButton(control_panel, SC_Start, "Start"), wxSizerFlags().Expand().Border(wxALL, 10));
    control_line1_sizer->Add(new wxButton(control_panel, SC_Stop, "Stop"), wxSizerFlags().Expand().Border(wxALL, 10));
    control_line1_sizer->Add(new wxButton(control_panel, SC_Settings, "Settings"), wxSizerFlags().Expand().Border(wxALL, 10));
    wxBoxSizer *control_line2_sizer = new wxBoxSizer(wxHORIZONTAL);
    control_line2_sizer->Add(new wxButton(control_panel, SC_Bump, "Bump"), wxSizerFlags().Expand().Border(wxALL, 10));
    control_line2_sizer->Add(new wxButton(control_panel, SC_Avoid, "Avoid"), wxSizerFlags().Expand().Border(wxALL, 10));
    control_line2_sizer->Add(new wxButton(control_panel, SC_Reboot, "Reboot"), wxSizerFlags().Expand().Border(wxALL, 10));
    wxBoxSizer *control_line3_sizer = new wxBoxSizer(wxHORIZONTAL);
    control_line3_sizer->Add(new wxButton(control_panel, SC_Weather_Scan, "Weather Scan"), wxSizerFlags().Expand().Border(wxALL, 10));
    control_line3_sizer->Add(new wxButton(control_panel, SC_Update_Clock, "Update Clock"), wxSizerFlags().Expand().Border(wxALL, 10));
    control_sizer->Add(control_line1_sizer);
    control_sizer->Add(control_line2_sizer);
    control_sizer->Add(control_line3_sizer);
    control_panel->SetSizerAndFit(control_sizer);

    // Put it all together
    wxBoxSizer *frame_sizer = new wxBoxSizer(wxHORIZONTAL);
    frame_sizer->Add(m_status_panel, 2, wxEXPAND | wxALL, 2);
    frame_sizer->Add(control_panel, 1, wxEXPAND | wxALL, 2);
    m_status_panel->Layout();
    m_status_panel->Refresh();
    m_status_panel->Update();

    this->SetSizerAndFit(frame_sizer);


}

void SCFrame::startScanner(wxCommandEvent& e){
    if(m_audio->isStarted() || m_ss->isStarted()){
        wxMessageBox("A session is already running.", "Error",
                 wxOK | wxICON_ERROR,  this);
        return;
    }
    m_audio->setAudioDevice(m_audio->getDefaultAudioDevice());
    {
        wxBusyCursor waiting_visualization;  // will display spinning animation until out of scope
        m_audio->start();
    }
    if(m_audio->connectionFail()){
        m_audio->stop();
        wxMessageBox("Cannot connect to the scanner audio service.\n"
                    "Verify scanner is on and IP address is accurate.\n"
                    "Verify no other software is connected to the scanner.", 
                    "Connection Error",
                    wxOK | wxICON_ERROR, this);
        return;
    }
    wxBusyCursor waiting_visualization;  // will display spinning animation until out of scope
    updateStatus();
}

void SCFrame::stopScanner(wxCommandEvent& e){
    m_audio->stop();
    m_ss->stop();
}

void SCFrame::editSettings(wxCommandEvent& e){
    SettingsDialog dialog(this, m_settingsData, e.GetId());
    dialog.ShowModal();
}

void SCFrame::updateStatus(){
    m_ss->start([this] (std::shared_ptr<sc::SC_STATUS> cs){

        wxGetApp().CallAfter([this, cs](){

            // initialize with default colors
            this->m_mid_status_text[0]->SetOwnBackgroundColour(m_black);
            this->m_mid_status_text[0]->SetOwnForegroundColour(m_red);
            this->m_mid_status_text[2]->SetOwnBackgroundColour(m_black);
            this->m_mid_status_text[2]->SetOwnForegroundColour(m_orange);
            this->m_mid_status_text[4]->SetOwnBackgroundColour(m_black);
            this->m_mid_status_text[4]->SetOwnForegroundColour(m_yellow);

            if(cs->V_Screen == "conventional_scan" || cs->V_Screen == "trunk_scan"){

                // Check for System Hold
                if(cs->System_Hold == "On"){
                    this->m_mid_status_text[0]->SetOwnBackgroundColour(wxColor(182, 50, 43));
                    this->m_mid_status_text[0]->SetOwnForegroundColour(m_black);
                }

                // Check for Department Hold
                if(cs->Dept_Hold == "On"){
                    this->m_mid_status_text[2]->SetOwnBackgroundColour(m_orange);
                    this->m_mid_status_text[2]->SetOwnForegroundColour(m_black);
                }
            }
            if(cs->V_Screen == "conventional_scan"){
                // check for Conventional Frequency Channel Hold
                if(cs->ConvFreq_Hold == "On"){
                    this->m_mid_status_text[4]->SetOwnBackgroundColour(m_yellow);
                    this->m_mid_status_text[4]->SetOwnForegroundColour(m_black);
                }

                this->m_mid_status_text[0]->SetLabelText(cs->System_Name.c_str());
                this->m_mid_status_text[1]->SetLabelText(cs->MonitorList_Name);
                this->m_mid_status_text[2]->SetLabelText(cs->Dept_Name.c_str());
                this->m_bot_right_status_text[2]->SetLabelText(("SQL: " + cs->Prop_SQL).c_str());
                if(!cs->Playing){
                    this->m_mid_status_text[3]->SetLabelText(cs->ConvFreq_Name);
                    this->m_mid_status_text[4]->SetLabelText(cs->OverWriteText.c_str());
                    this->m_mid_status_text[5]->SetLabelText("");
                    this->m_bot_left_status_text[0]->SetLabelText("---");
                    this->m_bot_right_status_text[0]->SetLabelText("---");
                    this->m_bot_right_status_text[3]->SetLabelText("---");
                    this->m_bot_right_status_text[4]->SetLabelText("---");
                }
                else{
                    this->m_mid_status_text[3]->SetLabelText("");
                    this->m_mid_status_text[4]->SetLabelText(cs->ConvFreq_Name.c_str());
                    this->m_mid_status_text[5]->SetLabelText((cs->ConvFreq_Freq + " " + cs->ConvFreq_Mod).c_str());
                    this->m_bot_left_status_text[0]->SetLabelText(cs->ConvFreq_SvcType.c_str());
                    this->m_bot_right_status_text[0]->SetLabelText(cs->ConvFreq_SAS.c_str());
                    this->m_bot_right_status_text[3]->SetLabelText(("Sig: " + cs->Prop_Sig).c_str());
                    this->m_bot_right_status_text[4]->SetLabelText(("RSSI: " + cs->Prop_Rssi + "dBm").c_str());

                }
            }
            if(cs->V_Screen == "trunk_scan"){
                // Check for Trunk Channel Hold
                if(cs->TGID_Hold == "On"){
                    this->m_mid_status_text[4]->SetOwnBackgroundColour(m_yellow);
                    this->m_mid_status_text[4]->SetOwnForegroundColour(m_black);
                }

                this->m_mid_status_text[0]->SetLabelText(cs->System_Name.c_str());
                this->m_mid_status_text[1]->SetLabelText(cs->MonitorList_Name.c_str());
                this->m_mid_status_text[2]->SetLabelText(cs->Dept_Name.c_str());
                this->m_mid_status_text[3]->SetLabelText(cs->Site_Name.c_str());
                this->m_bot_right_status_text[2]->SetLabelText(("SQL: " + cs->Prop_SQL).c_str());

                if(!cs->Playing){
                    this->m_mid_status_text[4]->SetLabelText(cs->OverWriteText.c_str());
                    this->m_mid_status_text[5]->SetLabelText("");
                    this->m_bot_left_status_text[0]->SetLabelText("---");
                    this->m_bot_right_status_text[0]->SetLabelText("---");
                    this->m_bot_right_status_text[1]->SetLabelText("---");
                    this->m_bot_right_status_text[3]->SetLabelText("---");
                    this->m_bot_right_status_text[4]->SetLabelText("---");
                }
                else{
                    this->m_mid_status_text[4]->SetLabelText(cs->TGID_Name.c_str());
                    this->m_mid_status_text[5]->SetLabelText(cs->TGID_TGID.c_str());
                    this->m_bot_left_status_text[0]->SetLabelText(cs->TGID_SvcType.c_str());
                    this->m_bot_left_status_text[4]->SetLabelText(cs->UnitID_Name.c_str());
                    this->m_bot_right_status_text[0]->SetLabelText((cs->Prop_P25Status + " " + cs->SiteFreq_SAS).c_str());
                    this->m_bot_right_status_text[1]->SetLabelText(cs->SiteFreq_Freq.c_str());
                    this->m_bot_right_status_text[3]->SetLabelText(("Sig: " + cs->Prop_Sig).c_str());
                    this->m_bot_right_status_text[4]->SetLabelText(("RSSI: " + cs->Prop_Rssi + "dBm").c_str());
                }
            }
            if(cs->V_Screen == "wx_alert"){
                for(auto& lt: this->m_mid_status_text){
                    lt->SetLabelText("");
                }
                for(auto& lt: this->m_bot_left_status_text){
                    lt->SetLabelText("");
                }
                for(auto& lt: this->m_bot_right_status_text){
                    lt->SetLabelText("");
                }

                this->m_mid_status_text[2]->SetLabelText(cs->wx_Mode);
                this->m_mid_status_text[4]->SetLabelText("CH " + cs->wx_CH_No + "   " + cs->wx_Freq);

                this->m_mid_status_text[0]->SetLabelText("");
                this->m_mid_status_text[1]->SetLabelText("");
                this->m_mid_status_text[3]->SetLabelText("");
            }

            this->m_mid_status_text[0]->Refresh();
            this->m_mid_status_text[2]->Refresh();
            this->m_mid_status_text[4]->Refresh();

        });
    } ); 
}

void SCFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);     
}

void SCFrame::OnClose(wxCloseEvent& event)
{
    spdlog::debug("Entering SCFrame::OnClose");
    m_audio->stop();
    spdlog::debug("Before m_ss->stop()");
    m_ss->stop();
    spdlog::debug("After m_ss->stop()");

    event.Skip();

    spdlog::debug("Exiting SCFrame::OnClose");
}

void SCFrame::systemHold(wxCommandEvent& event){
    m_control->send_system_hold();
}
void SCFrame::deptHold(wxCommandEvent& WXUNUSED(event)){
    m_control->send_dept_hold();
}
void SCFrame::channelHold(wxCommandEvent& WXUNUSED(event)){
    m_control->send_channel_hold();
}
void SCFrame::bump(wxCommandEvent& WXUNUSED(event)){
    m_control->send_bump();
}
void SCFrame::reboot(wxCommandEvent& WXUNUSED(event)){
    m_control->send_reboot();
}
void SCFrame::weatherScan(wxCommandEvent& WXUNUSED(event)){
    m_control->send_weather_scan();
}
void SCFrame::avoid(wxCommandEvent& WXUNUSED(event)){
    m_control->send_avoid();
}
void SCFrame::updateClock(wxCommandEvent& WXUNUSED(event)){
    m_control->updateClock();
}

void SCFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                    "%s\n\n%s\n\nA minimal scanner client running under %s.",
                    "ScannerClient",
                    "Copyright (c) Neil D. Harvey",
                    wxGetOsDescription()
                 ),
                 "About",
                 wxOK | wxICON_INFORMATION,
                 this);
}

