// Copyright (c) Neil D. Harvey

#include <string>
#include <fstream>
#include <iterator>
#include <span>
#include <spdlog/spdlog.h>
#include "network/g711.h"
#include "scannerclient/RTPSession.h"


namespace sc{

RTPSession::RTPSession(std::string scanner_ip, int listen_port):
                       m_rtp_sock(scanner_ip, 0, listen_port){
}

void RTPSession::writeraw(std::string packet){
    if(std::fstream fraw("rtp.packets", std::ios::binary | std::ios::app); fraw){
        fraw.write(packet.c_str(), packet.length());
    }
}

void RTPSession::run(){

    unsigned short pcm_out[400]{};
    // bind to the listen_port
    m_rtp_sock.bind();

    // receive 1000 RTP datagrams and write to a file, stripping the header
    for(int i = 0; i < 1000; i++) {
        spdlog::debug("About to recvfrom RTP");
        std::string pcmu_in = m_rtp_sock.recvfrom();
        spdlog::debug("Received {} bytes on RTP", pcmu_in.length());
        //writeraw(pcmu_in);
        //continue;  // skip the remainder for now
        if(pcmu_in.length() > 12)
            pcmu_in.erase(0,11);    // strip 12 byte RTP header  TODO: verify header size which can be larger
        long nsamples = ulaw_expand(pcmu_in, pcm_out);
        std::span<unsigned short> output(pcm_out, nsamples);

        if(std::fstream fs("rtp.pcm", std::ios::binary | std::ios::app); fs){
                spdlog::debug("About to write RTP; HDR:{}", pcmu_in.substr(0, 11));
                fs.write(reinterpret_cast<const char *>(&pcm_out[0]), nsamples*sizeof(short));
        }

    }


}


} // namespace
