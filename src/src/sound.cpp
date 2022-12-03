#include "sound.hpp"
#include "debug.hpp"
using namespace std;

unordered_set<string>  SoundEngine::assets = {
    SOUND_GB_BATTLE_MUSIC,
    SOUND_LIGHTNING,
    SOUND_SLAM,
    SOUND_GB_VICTORY_MUSIC,
    SOUND_FAILURE
};

const char* SoundEngine::sound_dir = "Assets/audio/";


#ifdef UNMUTED
ISoundEngine* SoundEngine::engine =  createIrrKlangDevice();
#else
ISoundEngine* SoundEngine::engine = nullptr;
#endif

ISound* SoundEngine::play3D(const string& name, const glm::vec3& pos,  bool is_loop, float volume){

    
    string file_path = sound_dir + name;
    ISound* res = engine->play3D(file_path.c_str(), vec3d<float>(pos.x, pos.y, pos.z), is_loop, false, true);
    res->setVolume(volume);
    return res;
}

ISound* SoundEngine::play2D(const string& name, bool is_loop, float volume){

    string file_path = sound_dir + name;
    ISound* res =  engine->play2D(file_path.c_str(), is_loop, false, true);
    res->setVolume(volume);
    return res;
}



