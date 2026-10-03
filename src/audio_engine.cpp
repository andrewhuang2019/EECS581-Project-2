#define AUDIO_ENGINE_IMPL
#include "audio_engine.hpp"
#include <iostream>

AudioManager::AudioManager() {
    ma_result r = ma_engine_init(NULL, &engine);
    if (r != MA_SUCCESS) {
        std::cerr << "engine init failed: " << ma_result_description(r) << "\n";
    }
    ma_engine_set_volume(&engine, 0.5f);
}

AudioManager::~AudioManager() {
    ma_engine_uninit(&engine);
    
}

void AudioManager::play_sound(const char* audio_file) {
    ma_engine_play_sound(&engine, audio_file, NULL);
    ma_result r = ma_engine_play_sound(&engine, audio_file, NULL);
    if (r != MA_SUCCESS) {
        std::cerr << "play failed: " << ma_result_description(r) << "\n";
    }
}