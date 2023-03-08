// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <string>


namespace sc {
void trim (std::string& field);
void dump(const std::string fname, const std::string outbuf);
void stripctrlchars(std::string& msg);


} //namespace
 