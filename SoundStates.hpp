#ifndef __SOUND_STATES_HPP__
#define __SOUND_STATES_HPP__

#include <SDL3/SDL_stdinc.h>
#include <string>

struct SoundStates {
    Uint8* buffer;
    Uint32 length;
    Uint8* bufferStart;
    Uint32 originalLength;
    std::string name;
    int loop = 0;
    bool isInstrument = false;
};

#endif