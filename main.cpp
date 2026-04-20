#include "SoundSystem.hpp"
#include "sdl_manager.hpp"
#include <cmath>
#include <chrono>
#include <thread>
#include <cstdint>

using namespace std::chrono_literals;

void initialize();

int main(int argc, char** argv) {
    SDL_Manager &manager = SDL_Manager::sdl();

    SoundSystem tester;
    tester.loadSong("Songs/song.wav");
    tester.loadSong("Songs/infernal_galop.mp3");
    bool good = tester.playSong(0, 1);
    bool good2 = tester.playSong(1, 1);

    int instrument = 0;
    bool exit = false;
    int numKeys;
    SDL_Event e;

    while (!exit) {
        std::vector<SoundStates> playback = tester.getPlayback();
        int found = 0;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    exit = true;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    //Want some code here??
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    manager.spawnWindow("window", 300, 300, 0);
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    manager.closeWindow(e.window.windowID);
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (e.key.key == SDLK_G && e.key.repeat == 0) {
                        tester.playInstrument(instrument);
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    if (e.key.key == SDLK_G) {
                        tester.stopInstrument(instrument);
                    }
                    break;
                
            }
        }
        const bool* keyboardState = SDL_GetKeyboardState(&numKeys);
        if (keyboardState[SDL_SCANCODE_1]) {
            instrument = 0;
        }
        if (keyboardState[SDL_SCANCODE_2]) {
            instrument = 1;
        }
        if (keyboardState[SDL_SCANCODE_3]) {
            instrument = 2;
        }
        if (keyboardState[SDL_SCANCODE_4]) {
            instrument = 3;
        }
        if (keyboardState[SDL_SCANCODE_5]) {
            instrument = 4;
        }
        if (keyboardState[SDL_SCANCODE_6]) {
            instrument = 5;
        }
        // if (keyboardState[SDL_SCANCODE_G]) {
        //     tester.playInstrument(instrument);
        // }
        
        manager.updateWindows();
        // std::this_thread::sleep_for(1ms);
    }

    return 0;
}