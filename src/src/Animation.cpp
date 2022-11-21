#include "Animation.hpp"
#include "debug.hpp"
#include <fstream>
#include <SceneNode.hpp>
#include "PlayerAI.hpp"
#include <unordered_map>
using namespace std;
using namespace glm;


/* Animator */

void static clear_ani(Animation& ani){
    ani.m_curr_frame_idx = 0;
    ani.m_frames.clear();
}

void static update_ani(Animation& ani, float ts){
    assert(ani.m_curr_frame_idx < ani.m_frames.size());
    if (ts > ani.m_frames[ani.m_curr_frame_idx+1].time_stamp){
        ani.m_curr_frame_idx++;
    }
}

static float calcProgression(float curr_ts, KeyFrame * f1, KeyFrame* f2){
    float curr_prog =  curr_ts - f1->time_stamp;
    float period = f2->time_stamp - f1->time_stamp;
    //cout << "curr prog is " << curr_prog << " period is " << period << endl;
    assert(curr_prog >= 0);
    assert(period >= 0);
    return curr_prog / period;
}

static void print_pose(const unordered_map<SceneNode*, mat4>& poses){
    //cout << "Printing poses " << endl;
    for (auto& p : poses){
        cout << p.first->m_name << "->" << endl;
        cout << p.second << endl;
    }
}

void Animator::apply_poses_to_nodes(const unordered_map<SceneNode*, mat4>& poses){
    for (auto& p : poses){
        mat4 trans =  m_node->trans * p.second * inverse(p.first->rotationAndTransl);
        p.first->applyRotTranslTransform(trans);
    }
}

std::unordered_map<SceneNode*, mat4> Animator::calculateCurrPose(KeyFrame * f1, KeyFrame* f2){
    float progression = calcProgression(m_anim_time, f1, f2);
    //cout << "progression is " << progression << endl;
    std::unordered_map<SceneNode*, mat4> poses;
    for (auto & p : f1->transforms){
        if (f2->transforms.find(p.first) == f2->transforms.end()) continue;
        poses[p.first] = JointTransform::interpolate(p.second, f2->transforms[p.first], progression);
        if (p.first == m_node.get()){
            cout << "mat is " << poses[p.first] << endl;
        }
    }
    return move(poses);
}

void Animator::update(){
    if (m_has_anim == false) {
        //DLOG("no ani here!");
        return;
    }
    
    // increase animation time
    increase_animation_time();

    if (m_anim_time >= m_curr_anim.m_end_stamp) {
        DLOG("done ani");
        m_has_anim = false;
        m_anim_time = 0;
        
        return;
    }
    // update animation frame idx if possible
    update_ani(m_curr_anim, m_anim_time);
    //cout << "curr anim frame idx is " << m_curr_anim.m_curr_frame_idx << " curr ts is " << m_anim_time << " end ts is " << m_curr_anim.m_end_stamp << endl;

    KeyFrame* f1;
    KeyFrame* f2;
    // get current and next keyframe
    get_prev_next_keyframe(&f1, &f2);
    // cout << "curr frame is " << f1->time_stamp << " next frame is " << f2->time_stamp << endl;


    std::unordered_map<SceneNode*, mat4> trans = calculateCurrPose(f1, f2);
    //print_pose(trans);

    apply_poses_to_nodes(trans);

    //cout << "curr anim time is " << m_anim_time << endl;

    
}

void Animator::do_animation(Player* p,  const Animation& anim){
    // clear previous ani
    clear_ani(m_curr_anim);
    m_anim_time = 0;
    // create frame 0
    JointMap map_0;
    for (auto& p : anim.m_frames[0].transforms){
        map_0[p.first] = JointTransform{inverse(m_node->trans)* p.first->rotationAndTransl};
    }
    assert(m_curr_anim.m_frames.size() == 0 && m_curr_anim.m_curr_frame_idx == 0);
    // copy anim
    m_curr_anim.m_frames.push_back(KeyFrame(map_0, 0.0f));
    m_curr_anim.m_frames.insert(m_curr_anim.m_frames.end(), anim.m_frames.begin(), anim.m_frames.end());
    m_curr_anim.m_name = anim.m_name;
    m_curr_anim.m_end_stamp = anim.m_end_stamp;
    m_curr_anim.m_curr_frame_idx = 0;
    cout << "curr anim is " << m_curr_anim << endl;
    m_curr_ts = get_curr_time();
    m_has_anim = true;
}










/* Animation Loader */
AnimationLoader::AnimationLoader(){
    load_asset(PIKACHU_ANI_ASSET, HumanPlayer::get_instance());
    load_asset(SNORLAX_ANI_ASSET, AI::get_instance());
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
    vec3 transl_vec;
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
                ifs >> transl_vec.x >> transl_vec.y >> transl_vec.z;
                //cout << "joint name is " << joint_name << "ts is "<<time_stamp<<  endl;
                SceneNode* joint_node = player->get_node_by_name(joint_name);
                assert(joint_node != nullptr);
                jointMap[joint_node] = JointTransform{translate(transl_vec) * toMat4(rotation_quat)};
            }   // for joints
            frames.emplace_back(jointMap, time_stamp);
       }    // for frames
       m_assets[ani_name] = Animation(ani_name, frames);
    }   // while animation
    
}




std::ostream &operator << (std::ostream & os, const Animation & ani){
    os << "Ani: " << ani.m_name << " end at " << ani.m_end_stamp;

    os << "key frames are: " << endl;;
    for (auto & frames : ani.m_frames){
        os << "ts : " << frames.time_stamp << " joint transforms are: " << endl;
        for (auto & trans : frames.transforms){
            assert(trans.first != nullptr);
            os << trans.first->m_name << "->" << " trans: " << trans.second.trans << endl;
        }
    }
    return os;
}




