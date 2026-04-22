#ifndef __SOUND_STATES_HPP__
#define __SOUND_STATES_HPP__

#include <SDL3/SDL_stdinc.h>
#include <string>

struct SoundStates {
    Sint16* bufferStart;
    Uint32 totalSamples;
    double currentPos = 0.0;
    float targetFreq = 220.0f;
    float baseFreq = 220.0f;
    std::string name;
    bool isInstrument = false;
    bool loop = false;
    bool active = true;
    double getSpeedRatio() const {
        return (double)targetFreq / baseFreq;
    }
    bool paused = false;
};

#endif