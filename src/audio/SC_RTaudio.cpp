// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <memory>
#include <string>
#include <tuple>
#include <future>
#include <spdlog/spdlog.h>

#include "network/RTSPSession.h"
#include "network/RTPSession.h"
#include "audio/AudioBuffer.h"
#include "SC_RTaudio.h"

namespace sc {

int rtaudio_callback( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data );            //forward



void SC_RTaudio::start(){
    if(m_started){
        spdlog::error("Attempt to start an active SC_RTauidio.  Invoke stop() first.");
        return;
    }
    m_started = true;
    if ( m_dac.getDeviceCount() < 1 ) {
        spdlog::error("\nNo audio devices found! Check your RtAudio build script.\n");
        m_started = false;
        return;
    }
    auto d_info =  m_dac.getDeviceInfo(m_audio_device);
    spdlog::debug("Audio output device:{}", d_info.name);
    if(d_info.outputChannels < 1){
        spdlog::error("Device has no output channels");
        m_started = false;
        return;
    }

    m_channels = (d_info.outputChannels > 1) ? 2 : 1;

    m_oParams.deviceId = m_audio_device;
    m_oParams.nChannels = m_channels;
    m_oParams.firstChannel = 0;  // offset

    m_rtsp_session_ptr = std::make_unique<RTSPSession>();
    m_audio_buffer_ptr = m_rtsp_session_ptr->getAudioBuffer();
    m_audio_buffer_ptr->setChannels(m_channels);

    spdlog::debug("Before starting RTSPSession");
    // RTSPSession will start two threads, one for RTSP and one for RTP
    m_connection_fail = false;
    std::promise<bool> rtsp_success_promise;
    std::future<bool> rtsp_success_future = rtsp_success_promise.get_future();
    m_rtsp_session_ptr->start(std::move( rtsp_success_promise));
    if(!rtsp_success_future.get()){
        spdlog::error("Unable to start RTSP Session");
        m_rtsp_session_ptr->stop();
        m_connection_fail = true;
        m_started = false;
        return;
    }


    try {
        if(m_audio_buffer_ptr != nullptr){
            m_dac.openStream( &m_oParams, NULL, RTAUDIO_FLOAT32, m_sample_rate, &m_buffer_frames, &rtaudio_callback, (void *)&m_audio_buffer_ptr );
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
        m_started = false;
        if(m_rtsp_session_ptr != nullptr){
            m_rtsp_session_ptr->stop();
            spdlog::debug("After RTSPSession::stop()");
        }
        m_dac.closeStream();
        spdlog::debug("After m_dac.closeStream()");
}

bool SC_RTaudio::isStarted() const {
    return m_started;
}

bool SC_RTaudio::connectionFail() const {
    return m_connection_fail;
}

void SC_RTaudio::mute(){

}

void SC_RTaudio::unmute(){

}

std::vector< std::tuple<int /* device ID */, std::string /* device name */, int /* # of channels */, bool /* default? */> > SC_RTaudio::getAudioDevices() {
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

unsigned int SC_RTaudio::getDefaultAudioDevice(){
    return m_dac.getDefaultOutputDevice();
}



int rtaudio_callback( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data ){
    spdlog::debug("In callback");
    // The buffer comes in as a pointer to a shared_ptr, we want the actual shared_ptr
    std::shared_ptr<AudioBuffer> audio_buf_ptr =  (*(std::shared_ptr<AudioBuffer>*) data);
    int channels = audio_buf_ptr->getChannels();

    float_buf buf;
    audio_buf_ptr->getAudio(buf);
    float* outbuf_ptr = (float*) outputBuffer;
    for(auto f: buf){
        *outbuf_ptr++ = f;
        if(channels == 2){ // add another sample to interleave for two channels
            *outbuf_ptr++ = f;
        }
    }

    if(audio_buf_ptr->getStopped()){
        spdlog::debug("callback stopped");
        return 2;
    }

  return 0;
}

} //namespace
