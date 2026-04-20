#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <string>

class Sound {
    private:
        Sint16* audioBuffer;
        Uint32 sampleCount;
        float baseFrequency;
        std::string name;
    
    public:
        Sound(Uint8*, Uint32, std::string, float);
        ~Sound();
        Sint16* getBuffer();
        Uint32 getSampleCount();
        float getBaseFreq();
        std::string getName();
};

#endif
