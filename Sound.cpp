#include "Sound.hpp"

Sound::Sound(Uint8* buf, Uint32 len, std::string soundName, float baseFreq = 220.0f) {
    this->audioBuffer = (Sint16*)buf;
    this->name = soundName;
    this->baseFrequency = baseFreq;
    int bytesPerFrame = sizeof(Sint16) * 2;
    this->sampleCount = len / bytesPerFrame;
}

// Sound::Sound(Uint8* audio_buf, Uint32 audio_len, std::string soundName) {
//     this->audioBuf = audio_buf;
//     this->audioLen = audio_len;
//     this->name = soundName;
// }

Sound::~Sound() {
    
}

Sint16* Sound::getBuffer() {
    return this->audioBuffer;
}

Uint32 Sound::getSampleCount() {
    return this->sampleCount;
}

float Sound::getBaseFreq() {
    return this->baseFrequency;
}

std::string Sound::getName() {
    return this->name;
}

// Uint8* Sound::getBuffer() {
//     return this->audioBuf;
// }

// Uint32 Sound::getLength() {
//     return this->audioLen;
// }

// std::string Sound::getName() {
//     return this->name;
// }

// void Sound::setPlaying(bool status) {
//     this->playing = status;
// }

// bool Sound::isPlaying() {
//     return this->playing;
// }