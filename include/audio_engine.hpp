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

    private:
        ma_engine engine;

};