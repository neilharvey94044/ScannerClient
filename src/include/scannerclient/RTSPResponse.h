// Copyright (c) Neil D. Harvey

#pragma once

#include <unordered_map>
#include <string>
#include <sstream>
#include <scannerclient/SessionDescription.h>
#include <scannerclient/RTSPRequest.h>

using namespace std;

namespace sc {

// limited to those header fields used by Uniden's scanner
enum RTSPHdrFld {
    STATUS,
    VERSION,
    CSEQ,
    CACHE_CONTROL,
    PUBLIC,
    SERVER,
    SUPPORTED,
    CONTENT_BASE,
    CONTENT_LENGTH,
    CONTENT_TYPE,
    TRANSPORT,
    SESSION,
    RTP_INFO
};

class RTSPResponse {
    public:
        RTSPResponse(RTSPMethod method_type, const string response );
        int getStatus() const;
        string getVersion() const;
        int getCSeq() const;
        string getHdrFld(RTSPHdrFld attr) const;
        size_t getFldCount() const;
        
 
    protected:
        int m_status;
        string m_version;
        RTSPMethod m_method_type;
        const string m_raw_response;
        unordered_map<RTSPHdrFld, string> m_hdr_flds;
        const static unordered_map<string, RTSPHdrFld> m_hdr_fld_map;
        unique_ptr<SessionDescription> m_sdp;
        void parse();
        void parseStatus(const string statusLine);
        void parseSDP(stringstream& sresponse);

};

} // namespace