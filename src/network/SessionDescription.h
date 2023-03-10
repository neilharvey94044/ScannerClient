// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include "network/MediaDescription.h"

namespace sc {

class SessionDescription {
    public:
        SessionDescription(std::stringstream& sresponse);
        std::string getAudioChannel() const;

    private:
        std::unordered_multimap<char, std::string> m_sdp_value;
        std::vector<std::unique_ptr<MediaDescription>> m_md_collection;
        
    protected:
        void parse(std::stringstream& sresponse);

};



} //namespace