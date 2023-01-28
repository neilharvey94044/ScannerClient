// Copyright (c) Neil D. Harvey

#pragma once


#include <tuple>
#include <rtaudio/RtAudio.h>
#include <scannerclient/sc.h>
#include "scannerclient/RTSPSession.h"
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
        virtual std::vector< std::tuple<int /* device ID */, std::string /* device name */, int /* # of channels */, bool /* default? */> > getOutputDevices();
        virtual void setAudioDevice(int audio_device);
        virtual ~SC_RTaudio() = default;

    private:
        std::shared_ptr<RTSPSession> m_rtsp_session_ptr;
        std::shared_ptr<AudioBuffer> m_audio_buffer_ptr;
        RtAudio m_dac;
        RtAudio::StreamParameters m_oParams;
        unsigned int m_audio_device{0};
        unsigned int m_channels{1};
        unsigned int m_sample_rate{8000};
        unsigned int m_buffer_frames{320};

};

} //namespace