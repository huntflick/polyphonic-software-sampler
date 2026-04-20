#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <string>

class Sound {
    private:
        Uint8* audioBuf;
        Uint32 audioLen;
        std::string name;
        bool playing = false;
    
    public:
        Sound(Uint8*, Uint32, std::string);
        ~Sound();
        Uint8* getBuffer();
        Uint32 getLength();
        std::string getName();
        void setPlaying(bool);
        bool isPlaying();
};

#endif
