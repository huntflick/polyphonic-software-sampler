#include "SoundSystem.hpp"
#include "sdl_manager.hpp"
#include <cmath>
#include <chrono>
#include <thread>
#include <cstdint>

using namespace std::chrono_literals;

int octaveOffset = 0;

float getFrequency(int semitone) {
    return 220.0f * pow(2.0f, (semitone + octaveOffset * 12) / 12.0f);
}

int main(int argc, char** argv) {
    SDL_Manager &manager = SDL_Manager::sdl();

    SoundSystem tester;
    tester.loadSong("Songs/song.wav");
    tester.playSong(0, true);
    tester.loadInstrument("Instruments/trumpet_a3.wav");

    int instrument = 0;
    bool exit = false;
    int numKeys;
    SDL_Event e;

    while (!exit) {
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
                    if (e.key.repeat == 0) {
                        float targetFreq = -1.0f;
                        switch (e.key.key) {
                            case SDLK_A:
                                targetFreq = getFrequency(-9);
                                break;
                            case SDLK_W:
                                targetFreq = getFrequency(-8);
                                break;
                            case SDLK_S:
                                targetFreq = getFrequency(-7);
                                break;
                            case SDLK_E:
                                targetFreq = getFrequency(-6);
                                break;
                            case SDLK_D:
                                targetFreq = getFrequency(-5);
                                break;
                            case SDLK_F:
                                targetFreq = getFrequency(-4);
                                break;
                            case SDLK_T:
                                targetFreq = getFrequency(-3);
                                break;
                            case SDLK_G:
                                targetFreq = getFrequency(-2);
                                break;
                            case SDLK_Y:
                                targetFreq = getFrequency(-1);
                                break;
                            case SDLK_H:
                                targetFreq = getFrequency(0);
                                break;
                            case SDLK_U:
                                targetFreq = getFrequency(1);
                                break;
                            case SDLK_J:
                                targetFreq = getFrequency(2);
                                break;
                            case SDLK_K:
                                targetFreq = getFrequency(3);
                                break;
                            case SDLK_O:
                                targetFreq = getFrequency(4);
                                break;
                            case SDLK_L:
                                targetFreq = getFrequency(5);
                                break;
                            case SDLK_P:
                                targetFreq = getFrequency(6);
                                break;
                            case SDLK_SEMICOLON:
                                targetFreq = getFrequency(7);
                                break;
                            case SDLK_APOSTROPHE:
                                targetFreq = getFrequency(8);
                                break;
                            case SDLK_Z:
                                if (octaveOffset > -3) {
                                    octaveOffset -= 1;
                                }
                                break;
                            case SDLK_X:
                                if (octaveOffset < 4) {
                                    octaveOffset += 1;
                                }
                                break;
                        }
                        if (targetFreq > 0) {
                            tester.playInstrument(instrument, targetFreq);
                        }
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    float targetFreq = -1.0f;
                    switch (e.key.key) {
                        case SDLK_A:
                            targetFreq = getFrequency(-9);
                            break;
                        case SDLK_W:
                            targetFreq = getFrequency(-8);
                            break;
                        case SDLK_S:
                            targetFreq = getFrequency(-7);
                            break;
                        case SDLK_E:
                            targetFreq = getFrequency(-6);
                            break;
                        case SDLK_D:
                            targetFreq = getFrequency(-5);
                            break;
                        case SDLK_F:
                            targetFreq = getFrequency(-4);
                            break;
                        case SDLK_T:
                            targetFreq = getFrequency(-3);
                            break;
                        case SDLK_G:
                            targetFreq = getFrequency(-2);
                            break;
                        case SDLK_Y:
                            targetFreq = getFrequency(-1);
                            break;
                        case SDLK_H:
                            targetFreq = getFrequency(0);
                            break;
                        case SDLK_U:
                            targetFreq = getFrequency(1);
                            break;
                        case SDLK_J:
                            targetFreq = getFrequency(2);
                            break;
                        case SDLK_K:
                            targetFreq = getFrequency(3);
                            break;
                        case SDLK_O:
                            targetFreq = getFrequency(4);
                            break;
                        case SDLK_L:
                            targetFreq = getFrequency(5);
                            break;
                        case SDLK_P:
                            targetFreq = getFrequency(6);
                            break;
                        case SDLK_SEMICOLON:
                            targetFreq = getFrequency(7);
                            break;
                        case SDLK_APOSTROPHE:
                            targetFreq = getFrequency(8);
                            break;
                    }
                    if (targetFreq > 0) {
                        tester.stopInstrument(instrument, targetFreq);
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
        
        manager.updateWindows();
    }

    return 0;
}