// Copyright (c) Neil D. Harvey

#include <cctype>
#include <spdlog/spdlog.h>
#include <scannerclient/MediaDescription.h>
#include <scannerclient/SessionDescription.h>

using namespace std;

namespace sc {

SessionDescription::SessionDescription(stringstream& sresponse){
    parse(sresponse);
}

void SessionDescription::parse(stringstream& sresponse){
    string line;
    while(getline(sresponse, line)){
        string sdp_value{};
        char sdp_type{};
        int i = 2;
        if(line.size() > 2 && isalpha(line[0])){
            sdp_type = line[0];

            while( !iscntrl(line[i]) && i < line.size())
                sdp_value += line[i++];

            m_sdp_value.insert(make_pair(sdp_type, sdp_value));
            spdlog::debug("sdp_type:{}  sdp_value:{}", sdp_type, sdp_value);
        }

        //if next line is a media description parse media descriptions
        if(sresponse.peek() == 'm'){
            m_md_collection.push_back(std::move( MediaDescription::parse(sresponse)));
        }

    }

}

string SessionDescription::getAudioChannel() const{

    for(auto& md: m_md_collection){
        if(md->getMediaType() == MediaDescription::media_type::audio) 
            return md->getAudioChannel();
    }

    return "";
}

}