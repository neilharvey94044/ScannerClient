// Copyright (c) Neil D. Harvey

#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <scannerclient/MediaDescription.h>

using namespace std;

namespace sc {

class SessionDescription {
    public:
        SessionDescription(stringstream& sresponse);
        string getSDPField(const char sdp_type) const;
        const SessionDescription& getSDP() const;


    protected:
        unordered_map<char, string> m_sdp_value;
        vector<unique_ptr<MediaDescription>> m_md_collection;
        void parse(stringstream& sresponse);

};



} //namespace