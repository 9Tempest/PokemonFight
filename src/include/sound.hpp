#pragma once


#include <irrKlang.h>
#include <string>
#include "include.hpp"
#include <unordered_set>
using namespace irrklang;


#define SOUND_LIGHTNING "lightning.wav"
#define SOUND_SLAM "slam.wav"

#define SOUND_FAILURE "failure.wav"


#define SOUND_GB_BATTLE_MUSIC "gb_battle.wav"
#define SOUND_GB_VICTORY_MUSIC "gb_victory.wav"
#define SOUND_GB_TITLE_MUSIC "gb_title.wav"

#define SOUND_ORIGIN_BATTLE_MUSIC "origin_battle.wav"
#define SOUND_ORIGIN_VICTORY_MUSIC "origin_victory.wav"
#define SOUND_ORIGIN_TITLE_MUSIC "origin_title.wav"

#define SOUND_SS_BATTLE_MUSIC "ss_battle.wav"
#define SOUND_SS_VICTORY_MUSIC "ss_victory.wav"
#define SOUND_SS_TITLE_MUSIC "ss_title.wav"


#define UNMUTED

class SoundEngine{
        static ISoundEngine *engine;
        static std::unordered_set<std::string> assets;
        static const char* sound_dir;
    public:
        static void init(){
            // engine = createIrrKlangDevice();
        }

        static irrklang::ISound* play3D(const std::string& name, const glm::vec3& pos, bool is_loop = false, float volume = 1.0f);
        static irrklang::ISound* play2D(const std::string& name, bool is_loop = false, float volume = 1.0f);
};

