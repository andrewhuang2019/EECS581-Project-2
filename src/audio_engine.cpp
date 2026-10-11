#define AUDIO_ENGINE_IMPL
#include "audio_engine.hpp"
#include <iostream>

AudioManager::AudioManager() {
    ma_result r = ma_engine_init(NULL, &engine);
    if (r != MA_SUCCESS) {
        std::cerr << "engine init failed: " << ma_result_description(r) << "\n";
    }
    ma_engine_set_volume(&engine, 1.0f);

    r = ma_sound_init_from_file(&engine, "./assets/tile.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &m_click);
    m_click_loaded = (r == MA_SUCCESS);
    if (r != MA_SUCCESS) {
        std::cerr << "click sound init failed: " << ma_result_description(r) << "\n";
    }
    r = ma_sound_init_from_file(&engine, "./assets/mine_explosion.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &m_bomb);
    m_bomb_loaded = (r == MA_SUCCESS);
    if (r != MA_SUCCESS) {
        std::cerr << "bomb sound init failed: " << ma_result_description(r) << "\n";
    }
    r = ma_sound_init_from_file(&engine, "./assets/start.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &m_start);
    m_start_loaded = (r == MA_SUCCESS);
    if (r != MA_SUCCESS) {
        std::cerr << "start sound init failed: " << ma_result_description(r) << "\n";
    }
    r = ma_sound_init_from_file(&engine, "./assets/win.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &m_win);
    m_win_loaded = (r == MA_SUCCESS);
    if (r != MA_SUCCESS) {
        std::cerr << "win sound init failed: " << ma_result_description(r) << "\n";
    }
    r = ma_sound_init_from_file(&engine, "./assets/lose.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &m_lose);
    m_lose_loaded = (r == MA_SUCCESS);
    if (r != MA_SUCCESS) {
        std::cerr << "lose sound init failed: " << ma_result_description(r) << "\n";
    }
    r = ma_sound_init_from_file(&engine, "./assets/flag.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &m_flag);
    m_flag_loaded = (r == MA_SUCCESS);
    if (r != MA_SUCCESS) {
        std::cerr << "flag sound init failed: " << ma_result_description(r) << "\n";
    }
    

    std::cerr << "AudioManager ctor\n";  
}

AudioManager::~AudioManager() {
    if (m_click_loaded) {
        ma_sound_uninit(&m_click);
    }
    if (m_bomb_loaded) {
        ma_sound_uninit(&m_bomb);
    }
    if (m_start_loaded) {
        ma_sound_uninit(&m_start);
    }
    if (m_win_loaded) {
        ma_sound_uninit(&m_win);
    }
    if (m_lose_loaded) {
        ma_sound_uninit(&m_lose);
    }
    if (m_flag_loaded) {
        ma_sound_uninit(&m_flag);
    }
    ma_engine_uninit(&engine);
    std::cerr << "AudioManager dtor\n";  
}

void AudioManager::play_click(){
    if (!m_click_loaded) return;

    ma_sound_stop(&m_click);
    ma_sound_seek_to_pcm_frame(&m_click, 0);
    ma_sound_start(&m_click);
}

void AudioManager::play_bomb() {
    if (!m_bomb_loaded) return;

    ma_sound_stop(&m_bomb);
    ma_sound_seek_to_pcm_frame(&m_bomb, 0);
    ma_sound_start(&m_bomb);
}

void AudioManager::play_start() {
    if (!m_start_loaded) return;

    ma_sound_stop(&m_start);
    ma_sound_seek_to_pcm_frame(&m_start, 0);
    ma_sound_start(&m_start);
}

void AudioManager::play_win() {
    if (!m_win_loaded) return;

    ma_sound_stop(&m_win);
    ma_sound_seek_to_pcm_frame(&m_win, 0);
    ma_sound_start(&m_win);
}

void AudioManager::play_lose() {
    if (!m_lose_loaded) return;

    ma_sound_stop(&m_lose);
    ma_sound_seek_to_pcm_frame(&m_lose, 0);
    ma_sound_start(&m_lose);
}

void AudioManager::play_flag() {
    if (!m_flag_loaded) return;

    ma_sound_stop(&m_flag);
    ma_sound_seek_to_pcm_frame(&m_flag, 0);
    ma_sound_start(&m_flag);
}