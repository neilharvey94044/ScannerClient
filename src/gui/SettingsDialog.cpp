// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+



#include <wx/fileconf.h>
#include <wx/spinctrl.h>
#include <wx/propdlg.h>
#include <wx/valgen.h>
#include <wx/bookctrl.h>
#include <wx/propgrid/propgrid.h>
#include <spdlog/spdlog.h>

#include "SettingsDialog.h"


wxIMPLEMENT_CLASS(SettingsDialog, wxPropertySheetDialog);


enum {
    SD_OK = wxID_OK,
    PGID  = wxID_LAST + 1
};

wxBEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
    EVT_BUTTON(SD_OK, SettingsDialog::onOK)
wxEND_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* win, SettingsData& settingsData, int dialogType)
    : m_settingsData(settingsData)
{
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);

    int tabImage1 = -1;
    int tabImage2 = -1;

    int resizeBorder = wxRESIZE_BORDER;

  
    Create(win, wxID_ANY, "Preferences", wxDefaultPosition, wxDefaultSize,
           wxDEFAULT_DIALOG_STYLE | resizeBorder);

    CreateButtons(wxOK | wxCANCEL | wxHELP);

    wxBookCtrlBase* notebook = GetBookCtrl();
    notebook->SetImageList(NULL);

    wxPanel* generalSettings = CreateGeneralSettingsPage(notebook);
    wxPanel* advancedSettings = CreateAdvancedSettingsPage(notebook);

    notebook->AddPage(generalSettings, "General", true, tabImage1);
    notebook->AddPage(advancedSettings, "Advanced", false, tabImage2);

    LayoutDialog();
}


// General settings are:
//  - IP Address of Scanner
//  - Auto Start Checkbox


wxPanel* SettingsDialog::CreateGeneralSettingsPage(wxWindow* parent)
{

    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    //// IP Address of Scanner
    wxBoxSizer* itemSizer1 = new wxBoxSizer( wxHORIZONTAL );
    wxStaticText *staticText_IPlabel = new wxStaticText(panel, wxID_ANY, "IP Address");
    itemSizer1->Add(staticText_IPlabel, 0, wxALIGN_CENTER_VERTICAL);
    m_textIP = new wxTextCtrl(panel, wxID_ANY, "000.000.000.000", wxDefaultPosition, wxDefaultSize );  // add a validator
    //textIP->SetValidator(wxGenericValidator(&m_settingsData.m_loadLastOnStartup));
    itemSizer1->Add(m_textIP, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer1, 0, wxGROW|wxALL, 0);

    //// Auto Start Checkbox
    wxString autoStartLabel = "&Auto Start";
    wxBoxSizer* itemSizer2 = new wxBoxSizer( wxHORIZONTAL );
    m_checkBoxAutoStart = new wxCheckBox(panel, wxID_ANY, autoStartLabel, wxDefaultPosition, wxDefaultSize);
    itemSizer2->Add(m_checkBoxAutoStart, 0, wxGROW);
    item0->Add(itemSizer2, 0, wxGROW|wxALL, 0);
    m_checkBoxAutoStart->Hide();  //TODO: remove this when implemented

    topSizer->Add( item0, wxSizerFlags(1).Expand().Border(wxALL, 5) );
    panel->SetSizerAndFit(topSizer);

    // Populate with values
    wxConfigBase *pConfig = wxConfigBase::Get();
    if(pConfig != nullptr){
        spdlog::debug("Initializing settings");
        m_textIP->SetValue(pConfig->Read("/General/IPAddress", ""));
        m_checkBoxAutoStart->SetValue(pConfig->Read("/General/AutoStart", 1l) != 0);
    }

    return panel;
}

wxPanel* SettingsDialog::CreateAdvancedSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );


    wxPropertyGrid* pg = new wxPropertyGrid(panel, PGID,  wxDefaultPosition, wxSize(400,400), wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE );
    wxConfigBase *pConfig = wxConfigBase::Get();
    m_pg = pg;
    pg->Append( new wxIntProperty("audio_rtsp_port", wxPG_LABEL, pConfig->ReadLong("/Advanced/audio_rtsp_port", 554) ) );
    pg->Append( new wxIntProperty("status_udp_port", wxPG_LABEL, pConfig->ReadLong("/Advanced/status_udp_port", 50536) ) );
    pg->Append( new wxIntProperty("socket_read_wait", wxPG_LABEL, pConfig->ReadLong("/Advanced/socket_read_wait", 3000) ) );
    pg->Append( new wxIntProperty("socket_write_wait", wxPG_LABEL, pConfig->ReadLong("/Advanced/socket_write_wait",  3000) ) );
    pg->Append( new wxStringProperty("hint_rtp_port", wxPG_LABEL, pConfig->Read("/Advanced/hint_rtp_port", "49990-49991") ) );
    pg->Append( new wxIntProperty("debug_logging", wxPG_LABEL, pConfig->ReadLong("/Advanced/debug_logging", 0) ) );

    topSizer->Add(pg, 0, wxALIGN_LEFT);
    panel->SetSizerAndFit(topSizer);

    return panel;
}

void SettingsDialog::onOK(wxCommandEvent& e){
    wxConfigBase *pConfig = wxConfigBase::Get();
    if(pConfig != nullptr){
        spdlog::debug("Saving settings");

        // General Settings
        pConfig->Write("/General/IPAddress", m_textIP->GetValue());
        pConfig->Write("/General/AutoStart", m_checkBoxAutoStart->GetValue());

        // Advanced Settings
        if(m_pg != nullptr){
            pConfig->Write("/Advanced/audio_rtsp_port",   m_pg->GetPropertyByName("audio_rtsp_port")->GetValue().GetLong() );
            pConfig->Write("/Advanced/status_udp_port",   m_pg->GetPropertyByName("status_udp_port")->GetValue().GetLong() );
            pConfig->Write("/Advanced/socket_read_wait",  m_pg->GetPropertyByName("socket_read_wait")->GetValue().GetLong() );
            pConfig->Write("/Advanced/socket_write_wait", m_pg->GetPropertyByName("socket_write_wait")->GetValue().GetLong() );
            pConfig->Write("/Advanced/hint_rtp_port",     m_pg->GetPropertyByName("hint_rtp_port")->GetValue().GetString() );
            pConfig->Write("/Advanced/debug_logging",     m_pg->GetPropertyByName("debug_logging")->GetValue().GetLong() );
        }

        pConfig->Flush();

        spdlog::debug("After saving settings");
    }

    if ( Validate() && TransferDataFromWindow() )
    {
        if ( IsModal() )
            EndModal(wxID_OK);
        else
        {
            SetReturnCode(wxID_OK);
            this->Show(false);
        }
    }
}

