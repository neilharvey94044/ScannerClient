// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+


#include <cctype>
#include <unordered_map>
#include <memory>
#include <spdlog/spdlog.h>
#include "utils/utils.h"
#include "network/MediaDescription.h"


using namespace std;

namespace sc {

unique_ptr<MediaDescription> MediaDescription::parse(stringstream& sresponse) {
    //TODO: parse media description
    spdlog::debug("MediaDescription::parse");

    string line;
    unique_ptr<MediaDescription> mdPtr;
    while(getline(sresponse, line)){
        spdlog::debug("md line:{}", line);
        char md_type;
        string md_value;
        int equalPos = line.find_first_of('=');
        if(equalPos != string::npos){
            md_type = line.at(0);
            md_value = line.substr(equalPos +1);
            trim(md_value);
            if(md_type == 'm'){
                spdlog::debug("About to make a MediaDescription object");
                mdPtr = make_unique<MediaDescription>();
                mdPtr->m_md_value.insert(make_pair(md_type, md_value));

                // Parse the 'm' values
                vector<string> tokens;
                string token;
                stringstream stok(md_value);
                while(getline(stok, token, ' ')){
                    trim(token);
                    spdlog::debug("md m token:{}", token);
                    tokens.push_back(token);
                }
                if(tokens.size() > 3){
                    if(tokens.at(0) == "audio"){
                        mdPtr->m_media_type = media_type::audio;
                    }
                    mdPtr->m_transport_protocol = tokens.at(2);
                    if(tokens.at(3) == "0"){
                        mdPtr->m_media_format = media_format::PCMU;
                    }
                    
                }
            }
            else if(md_type == 'a'){
                int colonPos = md_value.find_first_of(':');
                if(colonPos != string::npos){
                    string k = md_value.substr(0, colonPos);
                    string v = md_value.substr(colonPos + 1);
                    trim(k); trim(v);
                    spdlog::debug("md(a) key:{}    md(a) value:{}", k, v);
                    if(k == "control" && mdPtr->m_media_type == media_type::audio){
                        mdPtr->m_audio_channel = v;
                    }
                }
            }

            int nextchar = sresponse.peek();
            if(iscntrl(nextchar) || nextchar == 'm')
            {
                return std::move(mdPtr);
            }
        }

    }
    return std::move(mdPtr);


}

enum MediaDescription::media_type MediaDescription::getMediaType() const {
    return m_media_type;
}
enum MediaDescription::media_format MediaDescription::getMediaFormat() const {
    return m_media_format;
}

string MediaDescription::getAudioChannel() const{
    return m_audio_channel;
}


} //namespace