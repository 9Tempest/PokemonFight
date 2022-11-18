#pragma once
#include <unordered_map>
#include <string>
#include "include.hpp"
#include <vector>

#define SNORLAX_ANI_ASSET "Assets/snorlax.ani"
#define PIKACHU_ANI_ASSET "Assets/pikachu.ani"

class SceneNode;
class Player;

class JointTransform{   

    public:
        glm::vec3 position;
        glm::quat rotation;
        //JointTransform(const glm::quat& rotation):rotation(rotation){}
        glm::mat4 get_transform_mat(){
            return glm::translate(position) * glm::toMat4(rotation);
        }
};

typedef std::unordered_map<SceneNode*, JointTransform> JointMap;

struct KeyFrame{
    
    
    public:
        JointMap transforms;
        float time_stamp;   // at time_stamp t, do transforms;
        KeyFrame(const JointMap& transforms, float time_stamp):transforms(transforms), time_stamp(time_stamp){}
};  

class Animation{

    friend std::ostream & operator << (std::ostream & os, const Animation & ani);
    public:
    std::string m_name;
    std::vector<KeyFrame> m_frames;
    float m_start_stamp;
    float m_end_stamp;
    uint m_curr_frame_idx;
    Animation( const std::string& name, const std::vector<KeyFrame> frames) 
        : m_name(name), m_frames(frames), m_start_stamp((*frames.begin()).time_stamp), m_end_stamp(frames.back().time_stamp), m_curr_frame_idx(0){}
    
    Animation( const Animation& other) 
        : m_name(other.m_name), m_frames(other.m_frames), m_start_stamp((*other.m_frames.begin()).time_stamp), m_end_stamp((*other.m_frames.end()).time_stamp), m_curr_frame_idx(0){}

    Animation(){}

};

class Animator{
    protected:
    std::shared_ptr<SceneNode> m_node;
    private:
    Animation* m_curr_anim;
    float m_anim_time;

    void increase_animation_time(){

    }

    void get_prev_next_keyframe(KeyFrame** prev_frame, KeyFrame** next_frame){
        *prev_frame = &m_curr_anim->m_frames[m_curr_anim->m_curr_frame_idx];
        assert(m_curr_anim->m_curr_frame_idx < m_curr_anim->m_frames.size()-1);
        *next_frame = &m_curr_anim->m_frames[m_curr_anim->m_curr_frame_idx+1];
    }

    public:
        Animator(std::shared_ptr<SceneNode> node): m_node(node){}
        void update();
        void do_animation(Animation& anim){
            m_curr_anim = &anim;
            m_anim_time = 0;
        }
};

class AnimationLoader{
    private:
        void print_assets();
        std::unordered_map<std::string, Animation> m_assets;
        void load_asset(const std::string& fname, Player* player);

         // add load animations from a file
        AnimationLoader();  
    public:
        static AnimationLoader* get_instance(){
            static AnimationLoader m_loader;
            return &m_loader;
        }

        Animation* get_animation_by_name(const std::string& name){
            if (m_assets.find(name) == m_assets.end()) return nullptr;
            return &m_assets[name];
        }


};