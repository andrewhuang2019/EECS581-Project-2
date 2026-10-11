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
        void play_start();
        void play_win();
        void play_lose();
        void play_flag();

    private:
        bool m_click_loaded = false;
        bool m_bomb_loaded = false;
        bool m_start_loaded = false;
        bool m_win_loaded = false;
        bool m_lose_loaded = false;
        bool m_flag_loaded = false;

        ma_engine engine;
        ma_sound m_click;
        ma_sound m_bomb;
        ma_sound m_start;
        ma_sound m_win;
        ma_sound m_lose;
        ma_sound m_flag;

};
#endif