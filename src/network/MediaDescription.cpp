// Copyright (c) Neil D. Harvey


#include <cctype>
#include <unordered_map>
#include <memory>
#include <spdlog/spdlog.h>
#include <scannerclient/MediaDescription.h>


using namespace std;

namespace sc {

MediaDescription::MediaDescription(const unordered_map<char, string>& md_v) : 
                            m_md_value{md_v}
                            {}

void MediaDescription::parse(stringstream& sresponse, vector<unique_ptr<MediaDescription>>&  md_collection) {
    //TODO: parse media descriptions
    spdlog::debug("MediaDescription::parse");
    unordered_map<char, string> md_values;

    string line;
    while(getline(sresponse, line)){
        char md_type;
        string md_value;
       int i = 2;
        if(line.size() > 2 && isalpha(line[0])){
            md_type = line[0];

            while(!iscntrl(line[i]) && i < line.size())
                md_value += line[i++];

            md_values[md_type] = md_value;
            spdlog::debug("md_type:{}  md_value:{}", md_type, md_value);
            int nextchar = sresponse.peek();
            if(iscntrl(nextchar) || nextchar == 'm')
            {
                spdlog::debug("Adding a MediaDescription with {} values", md_values.size());
                md_collection.push_back(make_unique<MediaDescription>(md_values));
            }
        }

    }
}

} //namespace