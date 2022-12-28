// Copyright (c) Neil D. Harvey

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>

using namespace std;

namespace sc{
class MediaDescription {
    public:
        // parses multiple Media Descriptions from sresponse and adds them to the md_collection
        static void parse(stringstream& sresponse, vector<unique_ptr<MediaDescription>>&  md_collection);
        MediaDescription(const unordered_map<char, string>& md_v);
                           
    protected:
        unordered_map<char, string> m_md_value;

};

}