// Copyright (c) Neil D. Harvey

#pragma once

#include <unordered_map>
#include <string>
#include <sstream>
#include "scannerclient/SessionDescription.h"
#include "scannerclient/RTSPRequest.h"

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
        RTSPResponse(RTSPMethod method_type, const std::string response );
        int getStatus() const;
        std::string getVersion() const;
        int getCSeq() const;
        std::string getSession() const;
        int getRTPPort() const;
        std::string getAudioChannel() const;
        std::string getHdrFld(RTSPHdrFld attr) const;
        size_t getFldCount() const;
        
 
    private:
        int m_status;
        int m_RTP_port;
        std::string m_version;
        std::string m_channel;
        RTSPMethod m_method_type;
        const std::string m_raw_response;
        std::unordered_map<RTSPHdrFld, std::string> m_hdr_flds;
        const static std::unordered_map<std::string, RTSPHdrFld> m_hdr_fld_map;
        std::unique_ptr<SessionDescription> m_sdp{nullptr};
        
    protected:
        void parse();
        void parseStatus(const std::string statusLine);
        void parseSDP(std::stringstream& sresponse);
        void parseTransport();

};

} // namespace