// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>

// for all others, include the necessary headers 
// includes almost all "standard" wxWidgets headers
#include "wx/wx.h"

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#include "gui/scApp.h"

wxIMPLEMENT_APP(SCApp);

bool SCApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    initializeLogger();

    // main application window
    m_frame = new SCFrame("Scanner Client");
    m_frame->SetThemeEnabled(true);

    // show the main application window
    m_frame->Show(true);

    spdlog::debug("SCApp::OnInit exiting");
    return true;
}


int SCApp::FilterEvent(wxEvent& event)
{
    if (event.GetEventType() == wxEVT_KEY_DOWN) {


        // Map escape key to "bump" event
        if (((wxKeyEvent&)event).GetKeyCode() == WXK_CONTROL) {
            spdlog::debug("Control Key Hit");
            m_frame->bump(*(new wxCommandEvent()));  // wxCommandEvent is not used
            return true;
        }
        if (((wxKeyEvent&)event).GetKeyCode() == WXK_SPACE) {
            spdlog::debug("Space Key Hit");
            m_frame->channelHold(*(new wxCommandEvent()));  // wxCommandEvent is not used
            return true;
        }

    }
    return -1;
}


void SCApp::initializeLogger(){
     auto custom_logger = spdlog::basic_logger_mt<spdlog::async_factory>("Custom", "sc.log");
    //auto custom_logger = spdlog::basic_logger_mt("Custom", "sc.log", true);
    spdlog::set_default_logger(custom_logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^[%l]%$ [%t] %v");
    spdlog::flush_on(spdlog::level::debug);
    spdlog::flush_every(std::chrono::seconds(10)); //doesn't seem to work
    spdlog::debug("Logger initialized");
}
