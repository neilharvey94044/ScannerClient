// Copyright (c) Neil D. Harvey

#pragma once

#include <unordered_map>
#include <string>
#include <sstream>

using namespace std;

namespace sc {

class SessionDescription {
    public:
        SessionDescription(stringstream& sresponse);
        string getSDPField(const char sdp_type) const;
        const SessionDescription& getSDP() const;


    protected:
        unordered_map<char, string> m_sdp_value;
        void parse(stringstream& sresponse);

};



} //namespace