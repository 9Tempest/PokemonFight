#pragma once
#include <unordered_map>
#include <string>
#include "include.hpp"
#include <vector>
#include "timestamp.hpp"
#include <deque>
#include <memory>
#define SNORLAX_ANI_ASSET "Assets/snorlax.ani"
#define PIKACHU_ANI_ASSET "Assets/pikachu.ani"

class SceneNode;
class Player;

class JointTransform{   

    public:
        glm::mat4 trans;
        //JointTransform(const glm::quat& rotation):rotation(rotation){}
        glm::mat4 get_transform_mat() const{
            return trans;
        }

        // interpolation between two transformations
        static glm::mat4 interpolate(const JointTransform& transA, const JointTransform& transB, float progression){
            return glm::interpolate(transA.get_transform_mat(), transB.get_transform_mat(), progression);
        }
};

// mapping from scenenode to transformation
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
    float m_end_stamp;
    uint m_curr_frame_idx;
    Animation( const std::string& name, const std::vector<KeyFrame> frames) 
        : m_name(name), m_frames(frames), m_end_stamp(frames.back().time_stamp), m_curr_frame_idx(0){}
    
    Animation( const Animation& other) 
        : m_name(other.m_name), m_frames(other.m_frames), m_end_stamp((*other.m_frames.end()).time_stamp), m_curr_frame_idx(0){}

    Animation():m_curr_frame_idx(0.0f){}


};

class Animator{
    protected:
    std::shared_ptr<SceneNode> m_node;
    private:
    bool m_has_anim = false;
    Animation m_curr_anim;
    float m_anim_time = 0.0f;
    float m_remaing_time = 0.0f;
    time_stamp m_curr_ts;

    // apply transformations to nodes
    void apply_poses_to_nodes(const std::unordered_map<SceneNode*, glm::mat4>& poses);

    // increase animation time
    void increase_animation_time(){
        float time_diff = get_time_diff(m_curr_ts, get_curr_time());
        m_anim_time = time_diff;
        m_remaing_time = m_curr_anim.m_end_stamp - time_diff;
    }

    // get the previous keyframe and next keyframe given current time
    void get_prev_next_keyframe(KeyFrame** prev_frame, KeyFrame** next_frame){
        *prev_frame = &m_curr_anim.m_frames[m_curr_anim.m_curr_frame_idx];
        assert(m_curr_anim.m_curr_frame_idx < m_curr_anim.m_frames.size()-1);
        *next_frame = &m_curr_anim.m_frames[m_curr_anim.m_curr_frame_idx+1];
    }

    std::unordered_map<SceneNode*, glm::mat4> calculateCurrPose(KeyFrame * f1, KeyFrame* f2);

    protected:
        void body_trans(glm::vec3& curr, const glm::vec3& tmp_curr,const glm::vec3& target);

    public:
        float get_remaining_time() const{
            return m_remaing_time;
        }
        bool get_has_anim() const{
            return m_has_anim;
        }
        float get_anim_len() const{
            return m_curr_anim.m_end_stamp;
        }
        Animator(std::shared_ptr<SceneNode> node): m_node(node){
            m_curr_ts = get_curr_time();
        }
        virtual void update();
        void do_animation(const Animation& anim);
};

// a singleton Loader that loads .ani file
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
