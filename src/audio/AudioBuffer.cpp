// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <array>
#include <spdlog/spdlog.h>
#include <samplerate.h>
#include "audio/AudioBuffer.h"

using namespace std;

namespace sc{

AudioBuffer::AudioBuffer(int starting_buffers){
    // create starting buffers
    for(int i = 0; i < starting_buffers; i++){
        rtp_buf_pool.push_back(make_shared<rtpbuf>());
        audio_buf_pool.push_back(make_shared<float_buf>());
    }

    // initialize libsamplerate
/*
    SRC_SINC_BEST_QUALITY       = 0,
    SRC_SINC_MEDIUM_QUALITY     = 1,
    SRC_SINC_FASTEST            = 2,
    SRC_ZERO_ORDER_HOLD         = 3,
    SRC_LINEAR                  = 4
*/
    int error{0};
    m_samplerate = src_new (SRC_SINC_FASTEST, 1, &error) ;
    if(error){
        spdlog::error("Error initializing libsamplerate: {}", src_strerror(error));
    }
}

	// provides a buffer for populating with audio
shared_ptr<rtpbuf> AudioBuffer::getBuf(){

    if(rtp_buf_pool.size() > 0){
        const std::lock_guard lock{rtp_buf_pool_mutex};
        auto buf_ptr = rtp_buf_pool.back();
        rtp_buf_pool.pop_back();
        return buf_ptr;
    }		
    else{
        auto buf_ptr = make_shared<rtpbuf>();
        (*buf_ptr).fill(0);
        return buf_ptr;
    }

}

	// puts a buffer containing audio into the pipeline for playing
void AudioBuffer::pushAudio(shared_ptr<rtpbuf> buf_ptr){

    // Get a float buffer to resample into
    shared_ptr<float_buf> float_buf_ptr;
    if(audio_buf_pool.size() > 0){
        const std::lock_guard lock{audio_buf_pool_mutex};
        float_buf_ptr = audio_buf_pool.back();
        audio_buf_pool.pop_back();
    }
    else{
        float_buf_ptr = make_shared<float_buf>();
        (*float_buf_ptr).fill(0);
    }

    // convert from short to float
    float_buf temp_buf{0};
    src_short_to_float_array ((short *) buf_ptr.get(), &temp_buf[0] , SC_SHORT_BUF_SIZE);

    // resample the audio
    SRC_DATA src_config;
    src_config.input_frames_used = 0;
    src_config.output_frames_gen = 0;
    src_config.data_in = &temp_buf[0];
    src_config.data_out = (float*) float_buf_ptr.get();
    src_config.output_frames = SC_FLOAT_BUF_SIZE;
    src_config.src_ratio = SC_SRC_RATIO;
    src_config.input_frames = SC_SHORT_BUF_SIZE;
    src_config.end_of_input = 0;
    src_process (m_samplerate, &src_config);

    {
        //put into pipeline for playing
        const std::lock_guard lock{audio_pipe_mutex};
        audio_pipe.push_back(float_buf_ptr);
    }

    {
        //return rtp buffer to the pool
        (*buf_ptr).fill(0);
        const std::lock_guard lock{rtp_buf_pool_mutex};
        rtp_buf_pool.push_back(buf_ptr);
    }

    //audio_pipe_max = audio_pipe.size() > audio_pipe_max ? audio_pipe.size() : audio_pipe_max;

}

	// writes the next audio buffer to buf_out for playing
	// and recycles the buffer
void AudioBuffer::getAudio(float_buf& buf_out){
    const std::lock_guard lock {audio_pipe_mutex};

    if(audio_pipe.size() < 1){
        spdlog::error("No Audio Available Yet");
        buf_out.fill(0);
        return;
    }
    auto buf_ptr = *audio_pipe.begin();
    std::copy( (*buf_ptr).begin(), (*buf_ptr).end(), buf_out.begin()  );
    audio_pipe.pop_front();
    (*buf_ptr).fill(0);

    {
        const std::lock_guard lock{audio_buf_pool_mutex};
        audio_buf_pool.push_back(buf_ptr);
        audio_buf_pool_max = (audio_buf_pool.size() > audio_buf_pool_max) ? audio_buf_pool.size() : audio_buf_pool_max;
    }

}

void AudioBuffer::setStopped(bool stopped){
    m_stopped = stopped;
}

bool AudioBuffer::getStopped() const{
    return m_stopped;
}

void AudioBuffer::setChannels(int channels){
    m_channels = channels;
}

int AudioBuffer::getChannels() const{
    return m_channels;
}

AudioBuffer::~AudioBuffer(){
     src_delete (m_samplerate);
}

} //namespace