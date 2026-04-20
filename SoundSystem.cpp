#include "SoundSystem.hpp"

SoundSystem::SoundSystem() {
    this->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL, SoundSystem::callback, this);
    if (!(this->stream)) {
        std::cerr << SDL_GetError();
        exit(1);
    }
    SDL_AudioDeviceID device = SDL_GetAudioStreamDevice(this->stream);
    if (device == 0) {
        std::cerr << SDL_GetError();
        exit(1);
    }
    int sampleFrames;
    bool formatSuccess = SDL_GetAudioDeviceFormat(device, &this->spec, &sampleFrames);
    if (!formatSuccess) {
        std::cerr << SDL_GetError();
        exit(1);
    }
    bool unpause = SDL_ResumeAudioStreamDevice(this->stream);
    if (!unpause) {
        std::cerr << SDL_GetError();
        exit(1);
    }
    float frequency = 220.0f;
    float duration = 0.1f;
    int sampleRate = this->spec.freq;
    int numSamples = static_cast<int>(duration * sampleRate);
    int bytesPerSample = SDL_AUDIO_BYTESIZE(this->spec.format);
    Uint32 bufferSize = numSamples * bytesPerSample * this->spec.channels;
    Uint8* audioBuf = (Uint8*)SDL_malloc(bufferSize);
    Sint16* sBuf = (Sint16*)audioBuf;
    for (int i = 0; i < numSamples; ++i) {
        Sint16 val = static_cast<Sint16>(32767 * 0.5 * sin(2.0 * M_PI * frequency * i / sampleRate));
        for (int c = 0; c < this->spec.channels; ++c) {
            sBuf[i * this->spec.channels + c] = val;
        }
    }
    this->instruments.push_back(Sound(audioBuf, bufferSize, "sinWave"));
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
    bool success = true;
    Uint8* audioBuf = nullptr;
    Uint32 audioLen = 0;
    SDL_AudioSpec fileSpec;
    bool loaded = SDL_LoadWAV(filePath.c_str(), &fileSpec, &audioBuf, &audioLen);
    if (!loaded) {
        std::cerr << SDL_GetError() << '\n';
        return false;
    }
    int convertedLenHolder = 0;
    bool convert = SDL_ConvertAudioSamples(&fileSpec, audioBuf, audioLen, &this->spec, &audioBuf, &convertedLenHolder);
    if (!convert) {
        std::cerr << SDL_GetError();
        SDL_free(audioBuf);
        return false;
    }
    audioLen = static_cast<Uint32>(convertedLenHolder);
    this->songs.push_back(Sound(audioBuf, audioLen, filePath));

    return this->songs.size() - 1;
}

bool SoundSystem::playSong(int index, int loop) {
    if (index < this->songs.size()) {
        Sound requested = this->songs[index];
        SoundStates soundStruct;
        soundStruct.buffer = requested.getBuffer();
        soundStruct.length = requested.getLength();
        soundStruct.name = requested.getName();
        soundStruct.bufferStart = requested.getBuffer();
        soundStruct.originalLength = requested.getLength();
        if (loop != 0) {
            soundStruct.loop = 1;
        }
        this->playback.push_back(soundStruct);
        return true;
    }
    std::cerr << "Could not play song\n";
    return false;
}

bool SoundSystem::playSong(std::string filePath, int loop) {
    int loaded = loadSong(filePath);
    if (!loaded) {
        std::cerr << SDL_GetError();
        return false;
    }
    return this->playSong(loaded, loop);
}

int SoundSystem::loadInstrument(std::string filePath) {
    bool success = true;
    Uint8* audioBuf;
    Uint32 audioLen;
    SDL_AudioSpec fileSpec;
    bool loaded = SDL_LoadWAV(filePath.c_str(), &fileSpec, &audioBuf, &audioLen);
    if (!loaded) {
        std::cerr << SDL_GetError() << '\n';
        return false;
    }
    int convertedLenHolder = 0;
    bool convert = SDL_ConvertAudioSamples(&fileSpec, audioBuf, audioLen, &this->spec, &audioBuf, &convertedLenHolder);
    if (!convert) {
        std::cerr << SDL_GetError();
        return false;
    }
    audioLen = static_cast<Uint32>(convertedLenHolder);
    this->instruments.push_back(Sound(audioBuf, audioLen, filePath));

    return this->songs.size() - 1;
}

bool SoundSystem::playInstrument(int index) {
    if (index >= this->instruments.size()) {
        std::cerr << "Invalid instrument\n";
        return false;
    }
    Sound &requested = this->instruments[index];
    if (!requested.isPlaying()) {
        requested.setPlaying(true);
        SoundStates soundStruct;
        soundStruct.buffer = requested.getBuffer();
        soundStruct.length = requested.getLength();
        soundStruct.name = requested.getName();
        soundStruct.bufferStart = requested.getBuffer();
        soundStruct.originalLength = requested.getLength();
        soundStruct.isInstrument = true;
        this->playback.push_back(soundStruct);
    }
    return true;
}

bool SoundSystem::playInstrument(std::string filePath) {
    int loaded = loadSong(filePath);
    if (!loaded) {
        std::cerr << SDL_GetError();
        return false;
    }
    return this->playInstrument(loaded);
}

void SoundSystem::stopInstrument(int index) {
    if (index >= this->instruments.size()) {
        std::cerr << "Invalid instrument\n";
    }
    Sound &requested = this->instruments[index];
    if (requested.isPlaying()) {
        requested.setPlaying(false);
        for (SoundStates &soundStruct : this->playback) {
            if (soundStruct.name == requested.getName()) {
                soundStruct.isInstrument = false;
            }
        }
    }
}

std::vector<SoundStates> SoundSystem::getPlayback() {
    return this->playback;
}

void SoundSystem::callback(void* userdata, SDL_AudioStream* astream, int additional_amount, int total_amount) {
    SoundSystem* system = static_cast<SoundSystem*>(userdata);
    Uint8* mix = new Uint8[additional_amount]();
    auto sound = system->playback.begin();
    while (sound != system->playback.end()) {
        Uint32 totalMixed = 0;
        while (totalMixed < (Uint32)additional_amount) {
            Uint32 remaining = (Uint32)additional_amount - totalMixed;
            Uint32 mixData = std::min(sound->length, remaining);
            SDL_MixAudio(mix + totalMixed, sound->buffer, system->spec.format, mixData, 1.0);
            sound->buffer += mixData;
            sound->length -= mixData;
            totalMixed += mixData;
            if (sound->length <= 0) {
                if (sound->loop != 0 || sound->isInstrument) {
                    sound->buffer = sound->bufferStart;
                    sound->length = sound->originalLength;
                }
                else {
                    break;
                }
            }
        }
        if (sound->length <= 0 && !sound->isInstrument && sound->loop == 0) {
            sound = system->playback.erase(sound);
        }
        else {
            ++sound;
        }
    }
    SDL_PutAudioStreamData(astream, mix, additional_amount);
    if (mix) {
        delete[] mix;
    }
}

std::vector<Sound> SoundSystem::getSongBank() {
    return this->songs;
}

std::vector<Sound> SoundSystem::getInstrumentBank() {
    return this->instruments;
}