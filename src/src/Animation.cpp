#include "Animation.hpp"
#include "debug.hpp"
#include <fstream>
#include <SceneNode.hpp>
#include "PlayerAI.hpp"

using namespace std;
using namespace glm;

AnimationLoader::AnimationLoader(){
    load_asset(PIKACHU_ANI_ASSET, AI::get_instance());
    print_assets();
}

void AnimationLoader::print_assets(){
    for (auto& s : m_assets){
        std::cout << s.first << " -> " << s.second << std::endl;
    }
}


void AnimationLoader::load_asset(const string& fname, Player* player){
    ifstream ifs(fname);
    if (ifs.fail()){
        cerr << "file not exist " << fname << endl;
        abort();
    }
    string ani_name, joint_name;
    uint num_frames, num_joints;
    quat rotation_quat;
    float time_stamp;

    // todo add translation
    while (ifs >> ani_name)
    {
       ifs >> num_frames; // fetch number of frames
       std::vector<KeyFrame> frames;
       for (uint i = 0; i < num_frames; i++){
            // fetch time stamp and number of joints 
            ifs >> num_joints >> time_stamp;
            JointMap jointMap;
            for (uint j = 0; j < num_joints; j++){
                ifs >> joint_name;
                ifs >> rotation_quat.w >> rotation_quat.x >> rotation_quat.y >> rotation_quat.z;
                SceneNode* joint_node = player->get_node_by_name(joint_name);
                assert(joint_node != nullptr);
                jointMap[joint_node] = JointTransform{vec3() ,rotation_quat};
            }   // for joints
            frames.emplace_back(jointMap, time_stamp);
       }    // for frames
       m_assets[ani_name] = Animation(ani_name, frames);
    }   // while animation
    
}




std::ostream &operator << (std::ostream & os, const Animation & ani){
    os << "Ani: " << ani.m_name << " starts at " << ani.m_start_stamp << " end at " << ani.m_end_stamp;

    os << "key frames are: " << endl;;
    for (auto & frames : ani.m_frames){
        os << "ts : " << frames.time_stamp << " joint transforms are: ";
        for (auto & trans : frames.transforms){
            assert(trans.first != nullptr);
            os << trans.first->m_name << "->" << " quat: " << trans.second.rotation << " trans: " << trans.second.position;
        }
        os << endl;
    }
    return os;
}




