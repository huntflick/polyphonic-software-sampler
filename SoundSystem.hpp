#ifndef __SOUND_SYSTEM_HPP__
#define __SOUND_SYSTEM_HPP__

#include <vector>
#include <string>
#include <algorithm>
#include <SDL3/SDL.h>
#include <iostream>
#include <SDL3/SDL_stdinc.h>
#include "Sound.hpp"
#include "SoundStates.hpp"


static int current_sine_sample = 0;

class SoundSystem {
    private:
        std::vector<Sound> songs;
        std::vector<Sound> instruments;
        SDL_AudioStream* stream;
        SDL_AudioSpec spec;
        std::vector<SoundStates> playback;
        static void callback(void*, SDL_AudioStream*, int, int);
    
    public:
        SoundSystem();
        ~SoundSystem();
        int loadSong(std::string);
        bool playSong(int);
        void stopSong(int);
        // bool playSong(std::string, int);
        int loadInstrument(std::string);
        bool playInstrument(int, float);
        // bool playInstrument(std::string);
        void stopInstrument(int, float);
        std::vector<SoundStates> getPlayback();
        std::vector<Sound> getSongBank();
        std::vector<Sound> getInstrumentBank();
        std::string getSongName(int);
        std::string getInstrumentName(int);
        void togglePause(int);
};

#endif