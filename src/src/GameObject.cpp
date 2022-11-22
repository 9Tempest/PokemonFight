#include "GameObject.hpp"
#include "SceneNode.hpp"
#include "include.hpp"
#include "debug.hpp"

using namespace std;
using namespace glm;

GameObject::GameObject(std::string name, int hp, shared_ptr<SceneNode> node): Animator(node),m_name(name), m_hp(hp), m_status(Status::Idle){
    m_pos = vec3(m_node->translateM[3]);
    m_target_pos = m_pos;
}

void GameObject::move(float x, float z){
    DLOG("Object %s move x:%f z:%f", m_name.c_str(), x, z);
    assert(m_status == Status::Idle);
    m_target_pos = m_pos +  vec3(x, 0, z);
    m_tmp_pos = m_pos;
}

// ------------- Pikachu logics ------------

void Pikachu::update(){

    // if done animation, set status to idle
    if (!get_has_anim()){
        m_status = Status::Idle;
        return;
    }

    // move to target position
    body_trans(m_pos, m_tmp_pos, m_target_pos);
    // update animator
    Animator::update();
}

void Pikachu::move(float x, float y){
    GameObject::move(x, y);
    Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("pikachu_move");
    assert(ani_ptr != nullptr);
    do_animation(*ani_ptr);
}

AttackUnit* Pikachu::attack(const std::string& name, GameObject* target){
    assert(m_status == Status::Idle);
    DLOG("Pikachu %s attack enemy %s", m_name.c_str(), target->get_name().c_str());
    // add details here

    return nullptr;
}

void Pikachu::under_attack(AttackUnit* attackUnit){
    // add details here

}

Pikachu::~Pikachu(){

}


// ------------- Snorlax logics ------------

AttackUnit* Snorlax::attack(const std::string& name, GameObject* target){
    assert(m_status == Status::Idle);
    DLOG("Snorlax %s attack enemy %s", m_name.c_str(), target->get_name().c_str());
    // add details here

    return nullptr;
}

void Snorlax::under_attack(AttackUnit* attackUnit){
    // add details here

}

Snorlax::~Snorlax(){
    
}