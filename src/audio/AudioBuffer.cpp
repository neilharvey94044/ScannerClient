// Copyright (c) Neil D. Harvey

#include <spdlog/spdlog.h>
#include "audio/AudioBuffer.h"

using namespace std;

namespace sc{

AudioBuffer::AudioBuffer(int starting_buffers){
    for(int i = 0; i < starting_buffers; i++){
        buf_pool.push_back(make_shared<rtpbuf>());
    }
}

	// provides a buffer for populating with audio
shared_ptr<rtpbuf> AudioBuffer::getBuf(){

    if(buf_pool.size() > 0){
        const std::lock_guard lock{pool_mutex};
        auto buf_ptr = buf_pool.back();
        buf_pool.pop_back();
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
    const std::lock_guard lock{pipe_mutex};
    buf_pipe.push_back(buf_ptr);

    pipe_max = buf_pipe.size() > pipe_max ? buf_pipe.size() : pipe_max;

}

	// writes the next audio buffer to buf_out for playing
	// and recycles the buffer
void AudioBuffer::getAudio(rtpbuf& buf_out){
    const std::lock_guard lock {pipe_mutex};

    if(buf_pipe.size() < 1){
        spdlog::error("No Audio Available Yet");
        buf_out.fill(0);
        return;
    }
    auto buf_ptr = *buf_pipe.begin();
    std::copy( (*buf_ptr).begin(), (*buf_ptr).end(), buf_out.begin()  );
    buf_pipe.pop_front();
    (*buf_ptr).fill(0);

    {
        const std::lock_guard lock{pool_mutex};
        buf_pool.push_back(buf_ptr);
        pool_max = (buf_pool.size() > pool_max) ? buf_pool.size() : pool_max;
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



} //namespace