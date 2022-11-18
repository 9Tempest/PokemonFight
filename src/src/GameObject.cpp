#include "GameObject.hpp"
#include "SceneNode.hpp"
#include "include.hpp"
#include "debug.hpp"

using namespace std;
using namespace glm;

GameObject::GameObject(std::string name, int hp, shared_ptr<SceneNode> node): Animator(node),m_name(name), m_hp(hp){
    m_pos = m_node->translateM[3];
}

void GameObject::move(float x, float y){
    DLOG("Object %s move x:%f y:%f", m_name.c_str(), x, y);
    assert(m_status == Status::Idle);
    m_pos = glm::translate(vec3(x,y,0)) * m_pos;
    m_node->translate(vec3(x,y,0));
}

// ------------- Pikachu logics ------------

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