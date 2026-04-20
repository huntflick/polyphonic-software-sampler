#include "Sound.hpp"

Sound::Sound(Uint8* audio_buf, Uint32 audio_len, std::string soundName) {
    this->audioBuf = audio_buf;
    this->audioLen = audio_len;
    this->name = soundName;
}

Sound::~Sound() {
    
}

Uint8* Sound::getBuffer() {
    return this->audioBuf;
}

Uint32 Sound::getLength() {
    return this->audioLen;
}

std::string Sound::getName() {
    return this->name;
}

void Sound::setPlaying(bool status) {
    this->playing = status;
}

bool Sound::isPlaying() {
    return this->playing;
}