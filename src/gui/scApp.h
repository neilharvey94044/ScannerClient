// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once
#include <memory>
#include "wx/wx.h"
#include "gui/SCFrame.h"

class SCApp : public wxApp
{
    public:
        virtual bool OnInit() wxOVERRIDE;
        virtual int FilterEvent(wxEvent& event) wxOVERRIDE;
        virtual void initializeLogger();

    private:
        SCFrame *m_frame;
        std::unique_ptr<sc::SC_CONFIG> m_pConfig;

};

wxDECLARE_APP(SCApp);

