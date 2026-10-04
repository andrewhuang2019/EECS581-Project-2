#ifndef AUDIO_ENGINE_HPP
#define AUDIO_ENGINE_HPP
#ifdef AUDIO_ENGINE_IMPL
#define MINIAUDIO_IMPLEMENTATION
#endif
#include "miniaudio.h"

class AudioManager {
    public:
        AudioManager();
        ~AudioManager(); 
        void play_sound(const char* audio_file);
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;
        void play_click();
        void play_bomb();

    private:
        ma_engine engine;
        ma_sound m_click;
        ma_sound m_bomb;

};
#endif