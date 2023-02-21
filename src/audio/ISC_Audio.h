// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#pragma once

#include <vector>
#include <string>

namespace sc{

class ISC_Audio {

    public:
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void mute() = 0;
        virtual void unmute() = 0;
        virtual unsigned int getDefaultAudioDevice() = 0;
        virtual std::vector< std::tuple<int /* device ID */, std::string /* device name */, int /* # of channels */, bool /* default? */> > getAudioDevices() = 0;
        virtual void setAudioDevice(int audio_device) = 0;
        virtual ~ISC_Audio() = default;
};

}