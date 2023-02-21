// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include <string>
#include <wx/propgrid/propgrid.h>
#include <wx/propdlg.h>
#include "wx/wx.h"



// Struct containing properties edited by SettingsDialog.
struct SettingsData
{
    SettingsData() :
        m_IP_Address("0.0.0.0"),
        m_AutoStart(false)
    {
    }

    std::string m_IP_Address;
    bool m_AutoStart;
};

// Property sheet dialog
class SettingsDialog: public wxPropertySheetDialog
{
    wxDECLARE_CLASS(SettingsDialog);
public:
    SettingsDialog(wxWindow* parent, SettingsData& settingsData, int dialogType);
    ~SettingsDialog() = default;

    wxPanel* CreateGeneralSettingsPage(wxWindow* parent);
    wxPanel* CreateAdvancedSettingsPage(wxWindow* parent);

protected:

    enum {
        ID_IP_ADDRESS = 100,
        ID_AUTO_START
    };
    void onOK(wxCommandEvent& e);

    wxTextCtrl* m_textIP;
    wxCheckBox* m_checkBoxAutoStart;
    SettingsData& m_settingsData;
    wxPropertyGrid* m_pg;
    wxDECLARE_EVENT_TABLE();
};