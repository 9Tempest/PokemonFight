#pragma once


#include <irrKlang.h>
#include <string>
#include "include.hpp"
#include <unordered_set>
using namespace irrklang;

#define SOUND_BACKGROUND_MUSIC "background.wav"
#define SOUND_LIGHTNING "lightning.wav"
#define SOUND_SLAM "slam.wav"
#define SOUND_VICTORY "victory.wav"
#define SOUND_FAILURE "failure.wav"

#define UNMUTED

class SoundEngine{
        static ISoundEngine *engine;
        static std::unordered_set<std::string> assets;
        static const char* sound_dir;
    public:
        static void init(){
            // engine = createIrrKlangDevice();
        }

        static irrklang::ISound* play3D(const std::string& name, const glm::vec3& pos, bool is_loop = false);
        static irrklang::ISound* play2D(const std::string& name, bool is_loop = false);
};

