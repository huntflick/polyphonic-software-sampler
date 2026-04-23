#include "SoundSystem.hpp"

SoundSystem::SoundSystem() {
    SDL_AudioSpec desired;
    desired.format = SDL_AUDIO_S16;
    desired.channels = 2;
    desired.freq = 44100;
    this->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired, SoundSystem::callback, this);
    if (!(this->stream)) {
        std::cerr << SDL_GetError();
        exit(1);
    }
    int sampleFrames;
    SDL_GetAudioDeviceFormat(SDL_GetAudioStreamDevice(this->stream), &this->spec, &sampleFrames);
    SDL_ResumeAudioStreamDevice(this->stream);
    float frequency = 220.0f;
    float duration = 1.0f;
    int numSamples = static_cast<int>(duration * this->spec.freq);
    int bytesPerSample = SDL_AUDIO_BYTESIZE(this->spec.format);
    Uint32 bufferSize = numSamples * bytesPerSample * this->spec.channels;
    Uint8* audioBuf = (Uint8*)SDL_malloc(bufferSize);
    Sint16* sBuf = (Sint16*)audioBuf;
    for (int i = 0; i < numSamples; ++i) {
        Sint16 val = static_cast<Sint16>(32767 * 0.5 * sin(2.0 * M_PI * frequency * i / this->spec.freq));
        for (int c = 0; c < this->spec.channels; ++c) {
            sBuf[i * this->spec.channels + c] = val;
        }
    }
    this->instruments.push_back(Sound(audioBuf, bufferSize, "sinWave", 220.0f));
}

SoundSystem::~SoundSystem() {
    bool pause = SDL_PauseAudioStreamDevice(this->stream);
    if (!pause) {
        std::cerr << SDL_GetError();
        exit(1);
    }
    this->songs.clear();
    this->instruments.clear();
    SDL_DestroyAudioStream(this->stream);
}

int SoundSystem::loadSong(std::string filePath) {
    Uint8* audioBuf = nullptr;
    Uint32 audioLen = 0;
    SDL_AudioSpec fileSpec;
    bool loaded = SDL_LoadWAV(filePath.c_str(), &fileSpec, &audioBuf, &audioLen);
    if (!loaded) {
        std::cerr << SDL_GetError() << '\n';
        return -1;
    }
    Uint8* convertedBuf = nullptr;
    int convertedLen = 0;
    if (!SDL_ConvertAudioSamples(&fileSpec, audioBuf, audioLen, &this->spec, &convertedBuf, &convertedLen)) {
        std::cerr << SDL_GetError() << '\n';
        SDL_free(audioBuf);
        return -1;
    }
    SDL_free(audioBuf);
    std::string fileName = std::filesystem::path(filePath).filename().string();
    this->songs.push_back(Sound(convertedBuf, (Uint32)convertedLen, fileName, 1.0f));
    return this->songs.size() - 1;
}

bool SoundSystem::playSong(int index) {
    if (index >= this->songs.size()) {
        std::cerr << "Could not play song\n";
        return false;
    }
    Sound& requested = this->songs[index];
    SoundStates soundStruct;
    soundStruct.bufferStart = requested.getBuffer();
    soundStruct.totalSamples = requested.getSampleCount();
    soundStruct.currentPos = 0.0;
    soundStruct.baseFreq = 1.0f;
    soundStruct.targetFreq = 1.0f;
    soundStruct.name = requested.getName();
    soundStruct.loop = true;
    soundStruct.isInstrument = false;
    soundStruct.active = true;
    this->playback.push_back(soundStruct);
    return true;
}

void SoundSystem::stopSong(int index) {
    if (index >= this->songs.size()) {
        std::cerr << "Invalid song\n";
    }
    Sound &requested = this->songs[index];
    for (auto &sound : this->playback) {
        if (sound.name == requested.getName() && !sound.isInstrument) {
            sound.active = false;
        }
    }
}

int SoundSystem::loadInstrument(std::string filePath) {
    Uint8* audioBuf;
    Uint32 audioLen;
    SDL_AudioSpec fileSpec;
    bool loaded = SDL_LoadWAV(filePath.c_str(), &fileSpec, &audioBuf, &audioLen);
    if (!loaded) {
        std::cerr << SDL_GetError() << '\n';
        return -1;
    }
    Uint8* convertedBuf;
    int convertedLen;
    if (SDL_ConvertAudioSamples(&fileSpec, audioBuf, audioLen, &this->spec, &convertedBuf, &convertedLen)) {
        SDL_free(audioBuf);
        std::string fileName = std::filesystem::path(filePath).filename().string();
        this->instruments.push_back(Sound(convertedBuf, (Uint32)convertedLen, fileName, 220.0f));
    }
    else {
        std::cerr << "Could not load instrument " << filePath << '\n';
        return -1;
    }
    return (int)this->songs.size() - 1;
}

bool SoundSystem::playInstrument(int index, float targetFreq) {
    if (index >= this->instruments.size()) {
        std::cerr << "Invalid instrument\n";
        return false;
    }
    Sound &requested = this->instruments[index];
    SoundStates soundStruct;
    soundStruct.bufferStart = (Sint16*)requested.getBuffer();
    soundStruct.totalSamples = requested.getSampleCount();
    soundStruct.currentPos = 0.0f;
    soundStruct.targetFreq = targetFreq;
    soundStruct.baseFreq = requested.getBaseFreq();
    soundStruct.name = requested.getName();
    soundStruct.isInstrument = true;
    soundStruct.loop = true;
    soundStruct.active = true;
    this->playback.push_back(soundStruct);
    return true;
}

void SoundSystem::stopInstrument(int index, float freq) {
    if (index >= this->instruments.size()) {
        std::cerr << "Invalid instrument\n";
    }
    Sound &requested = this->instruments[index];
    for (auto &sound : this->playback) {
        if (sound.name == requested.getName() && sound.targetFreq == freq && sound.isInstrument) {
            // sound.loop = false;
            sound.active = false;
        }
    }
}

std::vector<SoundStates> SoundSystem::getPlayback() {
    return this->playback;
}

void SoundSystem::callback(void* userdata, SDL_AudioStream* astream, int additional_amount, int total_amount) {
    SoundSystem* system = static_cast<SoundSystem*>(userdata);
    int samplesReq = additional_amount / sizeof(Sint16);
    std::vector<float> mix(samplesReq, 0.0f);
    auto itr = system->playback.begin();
    while (itr != system->playback.end()) {
        if (itr->paused) {
            ++itr;
            continue;
        }
        double ratio = itr->getSpeedRatio();
        for (int i = 0; i < samplesReq; i += system->spec.channels) {
            if (!itr->active) {
                break;
            }
            int id1 = (int)itr->currentPos;
            if (id1 >= (int)itr->totalSamples - 1) {
                if (itr->loop) {
                    itr->currentPos = fmod(itr->currentPos, (double)itr->totalSamples - 1);
                    id1 = static_cast<int>(itr->currentPos);
                }
                else {
                    itr->active = false;
                    break;
                }
            }
            int id2 = (id1 + 1) % itr->totalSamples;
            if (id2 >= (int)itr->totalSamples) {
                id2 = (itr->loop) ? 0 : id1;
            }
            double fraction = itr->currentPos - id1;
            for (int c = 0; c < system->spec.channels; ++c) {
                int index1 = id1 * system->spec.channels + c;
                int index2 = id2 * system->spec.channels + c;
                float s1 = itr->bufferStart[index1];
                float s2 = itr->bufferStart[index2];
                float interpolated = s1 + (float)fraction * (s2 - s1);
                mix[i + c] += interpolated;
            }
            itr->currentPos += ratio;
        }
        if (!itr->active) {
            itr = system->playback.erase(itr);
        }
        else {
            ++itr;
        }
    }
    std::vector<Sint16> output(samplesReq);
    for (int i = 0; i < samplesReq; ++i) {
        float sample = mix[i];
        float vol = 0.4f;
        // if (sample > 32767.0f) {
        //     sample = 32767.0f;
        // }
        // else if (sample < -32768.0f) {
        //     sample = -32768.0f;
        // }
        output[i] = static_cast<Sint16>(sample * vol);
    }
    SDL_PutAudioStreamData(astream, output.data(), additional_amount);
}

std::vector<Sound> SoundSystem::getSongBank() {
    return this->songs;
}

std::vector<Sound> SoundSystem::getInstrumentBank() {
    return this->instruments;
}

std::string SoundSystem::getSongName(int songIndex) {
    if (songIndex >= this->songs.size()) {
        return "INVALID SONG";
    }
    return this->songs[songIndex].getName();
}

std::string SoundSystem::getInstrumentName(int instrumentIndex) {
    if (instrumentIndex >= this->instruments.size()) {
        return "INVALID INSTRUMENT";
    }
    return this->instruments[instrumentIndex].getName();
}

void SoundSystem::togglePause(int index) {
    if (index >= this->songs.size()) {
        std::cerr << "Invalid song\n";
    }
    Sound &requested = this->songs[index];
    for (auto &sound : this->playback) {
        if (sound.name == requested.getName() && !sound.isInstrument) {
            sound.paused = !sound.paused;
        }
    }
}