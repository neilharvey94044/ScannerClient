// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include <wx/confbase.h>
#include <atomic>

#include "config/SC_CONFIG.h"


namespace sc {

class WX_ConfigImpl : public SC_CONFIG {
    public:
        friend class SC_CONFIG;
        //friend std::unique_ptr<SC_CONFIG> std::make_unique<SC_CONFIG>();

    private:
        WX_ConfigImpl();
        wxConfigBase *m_pConfig;
        static std::atomic_bool m_initialized;

};

} // namespace
