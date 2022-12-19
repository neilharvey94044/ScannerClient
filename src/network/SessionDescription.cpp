// Copyright (c) Neil D. Harvey

#include <spdlog/spdlog.h>
#include <network/SessionDescription.h>

namespace sc {

SessionDescription::SessionDescription(stringstream& sresponse){
    parse(sresponse);
}

const SessionDescription& SessionDescription::getSDP() const{
    return *this;
}

void SessionDescription::parse(stringstream& sresponse){
    string line;
    while(getline(sresponse, line)){
        string sdp_value{};
        char sdp_type{};
        int i = 2;
        if(line.size() > 2 && isalpha(line[0])){
            sdp_type = line[0];

            while(line[i] != '\r' && line[i] != '\n')
                sdp_value += line[i++];

            m_sdp_value[sdp_type] = sdp_value;
            spdlog::debug("sdp_type:{}  sdp_value:{}", sdp_type, sdp_value);
        }

    }

}

string SessionDescription::getSDPField(const char sdp_type) const{
        return !((m_sdp_value.find(sdp_type)) == m_sdp_value.end()) ?  m_sdp_value.at(sdp_type) : "";

}

}