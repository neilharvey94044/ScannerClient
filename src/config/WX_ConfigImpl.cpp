// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <wx/wx.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include "config/WX_ConfigImpl.h"


namespace sc {
std::atomic_bool WX_ConfigImpl::m_initialized{false};

WX_ConfigImpl::WX_ConfigImpl()
{

    if(m_initialized){
        m_pConfig = wxConfigBase::Get();
    }
    else{
        //m_pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, "sc.cfg", wxEmptyString, wxCONFIG_USE_RELATIVE_PATH);
        m_pConfig = new wxFileConfig("ScannerClient", wxEmptyString, wxEmptyString, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
        m_pConfig->SetRecordDefaults(false);
        wxConfigBase::Set(m_pConfig);
        m_initialized = true;
    }

    load();

}

void WX_ConfigImpl::load(){
    // General Settings
    ip_address = m_pConfig->Read("/General/IPAddress", "");
    auto_start = m_pConfig->Read("/General/AutoStart", 0l);

    // Advanced Settings
    audio_rtsp_port = m_pConfig->Read("/Advanced/audio_rtsp_port", 554);
    status_udp_port = m_pConfig->Read("/Advanced/status_udp_port", 50536);
    socket_read_wait_ms = m_pConfig->Read("/Advanced/socket_read_wait", 1000);
    socket_write_wait_ms = m_pConfig->Read("/Advanced/socket_write_wait", 2000);
    hint_rtp_port = m_pConfig->Read("/Advanced/hint_rtp_port", "49990-49991");
    debug_logging = m_pConfig->ReadLong("/Advanced/debug_logging", 0);
}

} // namespace