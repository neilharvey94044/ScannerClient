// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once


#include <tuple>
#include <rtaudio/RtAudio.h>
#include "network/RTSPSession.h"
#include "audio/AudioBuffer.h"
#include "audio/ISC_Audio.h"

namespace sc{

class SC_RTaudio : ISC_Audio {

    public:
        SC_RTaudio() = default;
        virtual void start();
        virtual void stop();
        virtual void mute();
        virtual void unmute();
        virtual bool isStarted() const;
        virtual bool connectionFail() const;
        virtual unsigned int getDefaultAudioDevice();
        virtual std::vector< std::tuple<int /* device ID */, std::string /* device name */, int /* # of channels */, bool /* default? */> > getAudioDevices();
        virtual void setAudioDevice(int audio_device);
        virtual ~SC_RTaudio() = default;

    private:
        std::unique_ptr<RTSPSession> m_rtsp_session_ptr{nullptr};
        std::shared_ptr<AudioBuffer> m_audio_buffer_ptr{nullptr};
        RtAudio m_dac{};
        RtAudio::StreamParameters m_oParams;
        unsigned int m_audio_device{0};
        unsigned int m_channels{1};  // just for resampling; will use two channels for audio if available
        unsigned int m_sample_rate{44100};
        unsigned int m_buffer_frames{SC_FLOAT_BUF_SIZE};
        std::atomic_bool m_started{false};
        std::atomic_bool m_connection_fail{false};

};

} //namespace