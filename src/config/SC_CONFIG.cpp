// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <memory>

#include "config/SC_CONFIG.h"
#include "config/WX_ConfigImpl.h"


namespace sc {

std::unique_ptr<SC_CONFIG> const SC_CONFIG::get(){

    //TODO: support other configuration implementations here as needed

    return std::move(std::unique_ptr<WX_ConfigImpl>(new WX_ConfigImpl()));  // std::make_unique doesn't work with private constructor

}

}