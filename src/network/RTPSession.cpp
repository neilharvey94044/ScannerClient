// Copyright (c) Neil D. Harvey

extern "C" {
#if defined(_WIN32)
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0600
    #endif
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif
}

#include <string>
#include <array>
#include <fstream>
#include <iterator>
#include <memory>
#include <span>
#include <ranges>
#include <thread>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "scannerclient/sc.h"
#include "audio/AudioBuffer.h"
#include "network/g711.h"
#include "scannerclient/RTPSession.h"


namespace sc{

RTPSession::RTPSession(std::string scanner_ip, int listen_port, std::shared_ptr<AudioBuffer>& audio_buf_ptr):
                       m_rtp_sock(scanner_ip, 0, listen_port),
                       m_audio_buf_ptr(audio_buf_ptr)
                       {}

RTPHDRexpanded RTPSession::formatHeader(std::span<char> dgram){
    RTPHDRexpanded hdr{};
    if(dgram.size() > 12){

        unsigned char byte1{static_cast<unsigned char>(dgram[0])};
        unsigned char byte2{static_cast<unsigned char>(dgram[1])};
        hdr.version   =         ((byte1 &  0xC0) >> 6);            // Version
        hdr.padding   =         ((byte1 & 0x20) >> 5) == 1;        // Padding Bit
        hdr.extension =         ((byte1 & 0x10) >> 4) == 1;        // Extension (X Bit)
        hdr.CSRCcount =         ((byte1 & 0x0F));                  // CSRC Count
        hdr.marker =            ((byte2 & 0x80) >> 7) == 1;        // Marker Bit
        hdr.payload_type =      ((byte2 & 0x7F));                  // Payload Type

        memcpy(&hdr.sequence, &dgram[2], 2);    hdr.sequence = ntohs(hdr.sequence);
        memcpy(&hdr.timestamp, &dgram[4], 4);   hdr.timestamp = ntohl(hdr.timestamp);
        memcpy(&hdr.SSRC, &dgram[8], 4);        hdr.SSRC = ntohl(hdr.SSRC);
    }
    return hdr;
}


void RTPSession::writeraw(std::string dgram){
    if(std::fstream fraw("rtp.packets", std::ios::binary | std::ios::app); fraw){
        fraw.write(dgram.c_str(), dgram.length());
    }
}

void RTPSession::start(){
    m_rtp_thread = std::make_unique<std::thread>(&RTPSession::execute, this);
}

void RTPSession::execute(){
    spdlog::debug("RTPSession::execute entered");
    m_running = true;

    // bind to the listen_port obtained from RTSP
    m_rtp_sock.bind();

    // receive datagrams until signalled to stop
    while(m_running) {
        spdlog::debug("About to recvfrom RTP");
        auto pcm_out_ptr = m_audio_buf_ptr->getBuf();
        std::string pcmu_in = m_rtp_sock.recvfrom();
        RTPHDRexpanded hdr = formatHeader(std::span<char>(pcmu_in));
        spdlog::debug("Version:{} Padding_bit:{} Extension_bit:{} CSRC_count:{} Marker_bit:{} Payload_type:{} Sequence#:{} Timestamp:{} SSRC:{}",
                            hdr.version, hdr.padding, hdr.extension, hdr.CSRCcount, hdr.marker, hdr.payload_type, hdr.sequence, hdr.timestamp, hdr.SSRC );
        //TODO: optimize this code (e.g. remove erase)
        if(pcmu_in.length() > 12)
            pcmu_in.erase(0,12); // strip header
        long nsamples = ulaw_expand(pcmu_in, (*pcm_out_ptr));  // decompand
        m_audio_buf_ptr->pushAudio(pcm_out_ptr);               // put in pipeline for playing
    }

    m_audio_buf_ptr->setStopped(true);
}

void RTPSession::stop(){
    spdlog::info("RTPSession::stop invoked");
    m_running = false;
    m_rtp_thread->join();
}


} // namespace
