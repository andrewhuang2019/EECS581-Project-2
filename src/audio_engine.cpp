#define AUDIO_ENGINE_IMPL
#include "audio_engine.hpp"
#include <iostream>

AudioManager::AudioManager() {
    ma_result r = ma_engine_init(NULL, &engine);
    if (r != MA_SUCCESS) {
        std::cerr << "engine init failed: " << ma_result_description(r) << "\n";
    }
    ma_engine_set_volume(&engine, 1.0f);

    ma_sound_init_from_file(&engine, "./assets/blop.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &m_click);
    ma_sound_init_from_file(&engine, "./assets/mine_explosion.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &m_bomb);

    std::cerr << "AudioManager ctor\n";  
}

AudioManager::~AudioManager() {
    ma_sound_uninit(&m_click);  
    ma_sound_uninit(&m_bomb);  
    ma_engine_uninit(&engine);
    std::cerr << "AudioManager dtor\n";  
}

void AudioManager::play_click(){
    ma_sound_stop(&m_click);
    ma_sound_seek_to_pcm_frame(&m_click, 0);
    ma_sound_start(&m_click);
}

void AudioManager::play_bomb() {
    ma_sound_stop(&m_bomb);
    ma_sound_seek_to_pcm_frame(&m_bomb, 0);
    ma_sound_start(&m_bomb);
}