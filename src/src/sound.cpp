#include "sound.hpp"
#include "debug.hpp"
using namespace std;

unordered_set<string>  SoundEngine::assets = {
    SOUND_BACKGROUND_MUSIC,
    SOUND_LIGHTNING,
    SOUND_SLAM
};

ISoundEngine* SoundEngine::engine;

ISound* SoundEngine::play3D(const string& name, const glm::vec3& pos){
    if (assets.find(name) == assets.end()) {
        DLOG("3d audio asset %s is not supported!", name.c_str());
        return nullptr;
    }
    
    string file_path = sound_dir + name;
    ISound* res = engine->play3D(file_path.c_str(), vec3d<float>(pos.x, pos.y, pos.z), false, false, true);
    return res;
}

ISound* SoundEngine::play2D(const string& name, bool is_loop){
    if (assets.find(name) == assets.end()) {
        DLOG("3d audio asset %s is not supported!", name.c_str());
        return nullptr;
    }
    string file_path = sound_dir + name;
    return engine->play2D(file_path.c_str(), is_loop);
}



