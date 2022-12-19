// Copyright (c) Neil D. Harvey

#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <spdlog/spdlog.h>
#include <network/RTSPRequest.h>
#include <network/RTSPResponse.h>

namespace sc {

RTSPResponse::RTSPResponse(RTSPMethod method_type, const string response )
        : m_method_type {method_type},
          m_raw_response {response}
        {
            parse();
        }

void RTSPResponse::parse() {
    spdlog::debug("Running RTSPResponse::parse");
    stringstream sresponse(m_raw_response);
    string line;
    string statusline;
    getline(sresponse, statusline);
    parseStatus(statusline);
    bool bsdpcontent(false);
    // split remainder into lines then tokens
    while(getline(sresponse, line)){
        string hdrfield, hdrfieldvalue;
        int i = 0;
        //get RTSP header field name
        while(line[i] != ':') {
            hdrfield += line[i++];
        }

        // strip colon and leading whitespace
        while(line[i] == ':' || line[i] == ' ') i++;

        //get RTSP header field value
        while(line[i] != '\r' && line[i] != '\n'){
            hdrfieldvalue += line[i++];
        }
        m_hdr_flds[m_hdr_fld_map.at(hdrfield) ] = hdrfieldvalue;
        if (hdrfield == "Content-Type"){
            if (hdrfieldvalue == "application/sdp"){
                // Parse Session Description Protocol (SDP) content
                parseSDP(sresponse);
            }
            else break;
        }
    }

    for(auto& fld: m_hdr_flds)
        spdlog::debug("HDR field:{}  HDR value:{}", fld.first, fld.second);

}

void RTSPResponse::parseSDP(stringstream& sresponse){
    spdlog::debug("Parsing SDP");
    m_sdp = make_unique<SessionDescription>(sresponse);
}

void RTSPResponse::parseStatus(const string statusline){
    spdlog::debug("Parsing statusline: {}", statusline);
    string token;
    stringstream sline(statusline);
    vector<string> tokens;
    while(getline(sline, token, ' ')){
        tokens.push_back(token);
    }
    // should start with RTSP and should have at least 3 space delimited values
    if((statusline.compare(0, 4, "RTSP") == 0) && (tokens.size() >= 3)){
        m_version = tokens[0].substr(5, tokens[0].size() - 5);
        m_status = stoi(tokens[1]); 
    }
    else m_status = -1;

    spdlog::debug("version:{} status:{}", m_version, m_status);
}


int RTSPResponse::getStatus() const {
    return m_status;
}

string RTSPResponse::getVersion() const {
    return m_version;
}

int RTSPResponse::getCSeq() const {
    return stoi(getHdrFld(RTSPHdrFld::CSEQ));
}

// returns empty string if not found
string RTSPResponse::getHdrFld(RTSPHdrFld attr) const {
        return !((m_hdr_flds.find(attr)) == m_hdr_flds.end()) ?  m_hdr_flds.at(attr) : "";
}

// Currently only the fields required by the Uniden scanner.
//TODO: update for all RTSP header fields supported by RTSP/1.0
const unordered_map<string, RTSPHdrFld> RTSPResponse::m_hdr_fld_map   {
    {"CSeq", RTSPHdrFld::CSEQ},
    {"Cache-Control", RTSPHdrFld::CACHE_CONTROL},
    {"Public", RTSPHdrFld::PUBLIC},
    {"Server", RTSPHdrFld::SERVER},
    {"Supported", RTSPHdrFld::SUPPORTED},
    {"Content-Base", RTSPHdrFld::CONTENT_BASE},
    {"Content-Length", RTSPHdrFld::CONTENT_LENGTH},
    {"Content-Type", RTSPHdrFld::CONTENT_TYPE},
    {"Transport", RTSPHdrFld::TRANSPORT},
    {"Session", RTSPHdrFld::SESSION}
};


} //namespace