#include "GameObject.hpp"
#include "SceneNode.hpp"
#include "include.hpp"
#include "debug.hpp"
#include "PlayerAI.hpp"

using namespace std;
using namespace glm;

void GameObject::set_orientation(Orientation o){
      glm::vec3 axis = glm::vec3(0,1,0);
      float angle = 0;
      switch (o)
      {
      case Orientation::Left:
        angle = -90 - m_ori_angle;
        m_ori_angle = -90;
        break;
      case Orientation::Right:
        angle = 90 - m_ori_angle;
        m_ori_angle = 90;
        break;
      case Orientation::Down:
        angle = 0 - m_ori_angle;
        m_ori_angle = 0;
        break;
      case Orientation::Up:
        angle = 180 - m_ori_angle;
        m_ori_angle = 180;
      
      default:
        break;
      }
      

      m_node->rotatelocal('y', angle, m_node->trans);
      
    }

GameObject::GameObject(std::string name, int hp, shared_ptr<SceneNode> node): Animator(node), m_name(name), m_hp(hp), m_status(Status::Idle){
    m_pos = vec3(m_node->trans[3]);
    m_target_pos = m_pos;
}

void GameObject::move(float x, float z){
    DLOG("Object %s move x:%f z:%f", m_name.c_str(), x, z);
    assert(m_status == Status::Idle);
    m_status = Status::Moving;
    m_target_pos = m_pos +  vec3(x, 0, z);
    m_tmp_pos = m_pos;
}

void GameObject::set_pos(const glm::vec3& pos){
      vec3 offset = pos - m_pos;
      m_pos = pos;
      m_node->applyRotTranslTransform(glm::translate(offset));
    }

// ------------- Pikachu logics ------------

void Pikachu::update(){

    // if done animation, set status to idle
    if (!get_has_anim()){
        m_status = Status::Idle;
        return;
    }

    // move to target position
    if (m_status == Status::Moving){
         body_trans(m_pos, m_tmp_pos, m_target_pos);
    }
   
    // update animator
    Animator::update();
}

void Pikachu::move(float x, float y){
    GameObject::move(x, y);
    Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("pikachu_move");
    assert(ani_ptr != nullptr);
    do_animation(*ani_ptr);
}

void Pikachu::attack(const std::string& name, GameObject* target){
    assert(m_status == Status::Idle);
    DLOG("Pikachu %s attack enemy %s", m_name.c_str(), target->get_name().c_str());
    // add details here

    return;
}

void Pikachu::under_attack(AttackUnit* attackUnit){
    // add details here

}

Pikachu::~Pikachu(){

}


// ------------- Snorlax logics ------------

void Snorlax::attack(const std::string& name, GameObject* target){
    assert(m_status == Status::Idle);
    DLOG("Snorlax %s attack enemy %s", m_name.c_str(), target->get_name().c_str());
    // add details here
    m_status = Status::Attacking;
    m_attacku = new BodySlam(this, HumanPlayer::get_instance()->get_GameObject());
    return;
}

void Snorlax::under_attack(AttackUnit* attackUnit){
    // add details here

}

Snorlax::~Snorlax(){
    
}

void Snorlax::update(){
    


    if (!get_has_anim() && m_attacku == nullptr){
        m_status = Status::Idle;
        return;
    }

    // if done animation, set status to idle
    if (!get_has_anim() && m_attacku != nullptr){
        ((BodySlam*)m_attacku)->is_touch_sky = true;
    }
    
    // move to target position
    body_trans(m_pos, m_tmp_pos, m_target_pos);
    // update animator
    Animator::update();

    // update attackUnit
    if (m_attacku != nullptr){
        m_attacku->update(get_curr_time());
        if (m_attacku->get_is_done()){
            delete m_attacku;
            m_attacku = nullptr;
        }
    }
}