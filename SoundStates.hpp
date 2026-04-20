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
    // Uint8* buffer;
    // Uint32 length;
    // Uint8* bufferStart;
    // Uint32 originalLength;
    // std::string name;
    // int loop = 0;
    // bool isInstrument = false;
};

#endif