// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

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
};

wxDECLARE_APP(SCApp);

