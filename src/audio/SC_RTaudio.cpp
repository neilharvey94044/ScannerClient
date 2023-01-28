// Copyright (c) Neil D. Harvey

#include <memory>
#include <string>
#include <tuple>
#include <spdlog/spdlog.h>

#include "scannerclient/RTSPSession.h"
#include "scannerclient/RTPSession.h"
#include "audio/AudioBuffer.h"
#include "SC_RTaudio.h"

namespace sc {

int rtaudio_callback( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data );            //forward



void SC_RTaudio::start(){
    if ( m_dac.getDeviceCount() < 1 ) {
        spdlog::error("\nNo audio devices found! Check your RtAudio build script.\n");
        return;
    }
    auto d_info =  m_dac.getDeviceInfo(m_audio_device);
    spdlog::debug("Audio output device:{}", d_info.name);
    if(d_info.outputChannels < 1){
        spdlog::error("Device has no output channels");
        return;
    }

    m_channels = (d_info.outputChannels > 1) ? 2 : 1;

    m_oParams.deviceId = m_audio_device;
    m_oParams.nChannels = m_channels;
    m_oParams.firstChannel = 0;  // offset

    // will start two threads, one for RTSP and one for RTP
    m_rtsp_session_ptr = std::make_shared<RTSPSession>();
    m_audio_buffer_ptr = m_rtsp_session_ptr->getAudioBuffer();
    m_audio_buffer_ptr->setChannels(m_channels);
    spdlog::debug("Before starting RTSPSession");
    m_rtsp_session_ptr->start();
    spdlog::debug("After starting RTSPSession");

    try {
        if(m_audio_buffer_ptr != nullptr){
            m_dac.openStream( &m_oParams, NULL, RTAUDIO_SINT16, m_sample_rate, &m_buffer_frames, &rtaudio_callback, (void *)&m_audio_buffer_ptr );
            spdlog::debug("Opened Audio Stream");
            m_dac.startStream();
            spdlog::debug("Started Audio Stream");
        }

    }
    catch ( RtAudioError& e ) {
        spdlog::error("RtAudio Error: {}", e.getMessage());
  }

  spdlog::debug("Exiting SC_RTPaudio::start()");
}

void SC_RTaudio::stop(){
        spdlog::debug("Entering SC_RTPaudio::stop()");
        m_rtsp_session_ptr->stop();
}

void SC_RTaudio::mute(){

}

void SC_RTaudio::unmute(){

}

std::vector< std::tuple<int /* device ID */, std::string /* device name */, int /* # of channels */, bool /* default? */> > SC_RTaudio::getOutputDevices() {
    int device_count = m_dac.getDeviceCount();
    std::vector< std::tuple<int, std::string, int, bool> > devices;
    for(int d = 0; d < device_count; d++){
        auto d_info =  m_dac.getDeviceInfo(d);
        if(d_info.outputChannels > 0){
            devices.push_back(std::make_tuple(d, d_info.name, d_info.outputChannels, d_info.isDefaultOutput ));
        }
    }
    return std::move(devices);
}

void SC_RTaudio::setAudioDevice(int audio_device){
    m_audio_device = audio_device;
}



int rtaudio_callback( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data ){
    spdlog::debug("In callback");
    // The buffer comes in as a pointer to a shared_ptr, we want the actual shared_ptr
    std::shared_ptr<AudioBuffer> audio_buf_ptr =  (*(std::shared_ptr<AudioBuffer>*) data);
    int channels = audio_buf_ptr->getChannels();

    rtpbuf buf;
    audio_buf_ptr->getAudio(buf);
    unsigned short* outbuf_ptr = (unsigned short*) outputBuffer;
    for(unsigned short s: buf){
        *outbuf_ptr++ = s;
        if(channels == 2){ // add another sample to interleave for two channels
            *outbuf_ptr++ = s;
        }
    }

    if(audio_buf_ptr->getStopped()){
        spdlog::debug("callback stopped");
        return 2;
    }

  return 0;
}

} //namespace
