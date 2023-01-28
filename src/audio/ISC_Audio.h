// Copyright (c) Neil D. Harvey

#pragma once


namespace sc{

class ISC_Audio {

    public:
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void mute() = 0;
        virtual void unmute() = 0;
        virtual ~ISC_Audio() = default;
};

}