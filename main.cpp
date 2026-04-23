#include "SoundSystem.hpp"
#include "sdl_manager.hpp"
#include <cmath>
#include <chrono>
#include <thread>
#include <cstdint>
#include <map>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std::chrono_literals;

int octaveOffset = 0;
struct ActiveNote {
    int instrumentID;
    float freq;
};

float getFrequency(int semitone) {
    return 220.0f * pow(2.0f, (semitone + octaveOffset * 12) / 12.0f);
}

int main(int argc, char** argv) {
    SDL_Manager &manager = SDL_Manager::sdl();
    std::map<SDL_Keycode, ActiveNote> activeNotes;

    SoundSystem tester;

    const char* base = SDL_GetBasePath();
    if (!base) {
        return -1; 
    }
    std::string basePath(base);
    std::filesystem::path songsDir = std::filesystem::path(basePath) / "Songs";
    std::filesystem::path instrumentsDir = std::filesystem::path(basePath) / "Instruments";

    std::vector<std::string> songPaths;
    if (std::filesystem::exists(songsDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(songsDir)) {
            if (entry.path().extension() == ".wav") {
                songPaths.push_back(entry.path().string());
            }
        }
    }
    std::sort(songPaths.begin(), songPaths.end());
    for (const auto& path : songPaths) {
        tester.loadSong(path);
    }
    std::vector<std::string> instrumentPaths;
    if (std::filesystem::exists(instrumentsDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(instrumentsDir)) {
            if (entry.path().extension() == ".wav") {
                instrumentPaths.push_back(entry.path().string());
            }
        }
    }
    std::sort(instrumentPaths.begin(), instrumentPaths.end());
    for (const auto& path : instrumentPaths) {
        tester.loadInstrument(path);
    }
    tester.playSong(0);

    int instrument = 0;
    int song = 0;
    bool exit = false;
    SDL_Event e;
    while (!exit) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    exit = true;
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
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_W:
                                targetFreq = getFrequency(-8);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_S:
                                targetFreq = getFrequency(-7);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_E:
                                targetFreq = getFrequency(-6);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_D:
                                targetFreq = getFrequency(-5);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_F:
                                targetFreq = getFrequency(-4);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_T:
                                targetFreq = getFrequency(-3);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_G:
                                targetFreq = getFrequency(-2);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_Y:
                                targetFreq = getFrequency(-1);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_H:
                                targetFreq = getFrequency(0);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_U:
                                targetFreq = getFrequency(1);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_J:
                                targetFreq = getFrequency(2);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_K:
                                targetFreq = getFrequency(3);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_O:
                                targetFreq = getFrequency(4);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_L:
                                targetFreq = getFrequency(5);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_P:
                                targetFreq = getFrequency(6);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_SEMICOLON:
                                targetFreq = getFrequency(7);
                                activeNotes[e.key.key] = {instrument, targetFreq};
                                break;
                            case SDLK_APOSTROPHE:
                                targetFreq = getFrequency(8);
                                activeNotes[e.key.key] = {instrument, targetFreq};
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
                            case SDLK_0:
                                manager.spawnWindow("Keyboard", 700, 300, 0);
                                break;
                            case SDLK_1:
                                instrument = 0;
                                break;
                            case SDLK_2:
                                instrument = 1;
                                break;
                            case SDLK_3:
                                instrument = 2;
                                break;
                            case SDLK_4:
                                instrument = 3;
                                break;
                            case SDLK_5:
                                instrument = 4;
                                break;
                            case SDLK_LEFT:
                                tester.stopSong(song);
                                tester.playSong(song);
                                break;
                            case SDLK_DOWN:
                                tester.stopSong(song);
                                --song;
                                if (song < 0) {
                                    song = tester.getSongBank().size() - 1;
                                }
                                tester.playSong(song);
                                break;
                            case SDLK_RIGHT:
                                tester.stopSong(song);
                                ++song;
                                if (song > tester.getSongBank().size() - 1) {
                                    song = 0;
                                }
                                tester.playSong(song);
                                break;
                            case SDLK_UP:
                                tester.stopSong(song);
                                ++song;
                                if (song > tester.getSongBank().size() - 1) {
                                    song = 0;
                                }
                                tester.playSong(song);
                                break;
                            case SDLK_SPACE:
                                tester.togglePause(song);
                                break;
                        }
                        if (targetFreq > 0) {
                            tester.playInstrument(instrument, targetFreq);
                        }
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    ActiveNote note = activeNotes[e.key.key];
                    tester.stopInstrument(note.instrumentID, note.freq);
                    activeNotes.erase(e.key.key);
                    break;
                
            }
        }
        if (instrument > tester.getInstrumentBank().size() - 1) {
            instrument = tester.getInstrumentBank().size() - 1;
        }
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        std::string songName = tester.getSongName(song);
        int found = songName.find("/");
        if (found != std::string::npos) {
            songName = songName.substr(found + 1);
        }
        std::string instName = tester.getInstrumentName(instrument);
        int instFound = instName.find("/");
        if (instFound != std::string::npos) {
            instName = instName.substr(instFound + 1);
        }
        manager.renderUI(songName, instName, octaveOffset + 3);
        
        manager.updateWindows(octaveOffset);
    }

    return 0;
}