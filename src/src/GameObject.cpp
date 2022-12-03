#include "GameObject.hpp"
#include "SceneNode.hpp"
#include "include.hpp"
#include "debug.hpp"
#include "PlayerAI.hpp"
#include "Scene.hpp"
#include "GameWindow.hpp"
#include "random.hpp"
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
        m_ori = Orientation::Left;
        break;
      case Orientation::Right:
        angle = 90 - m_ori_angle;
        m_ori_angle = 90;
        m_ori = Orientation::Right;
        break;
      case Orientation::Down:
        angle = 0 - m_ori_angle;
        m_ori_angle = 0;
        m_ori = Orientation::Down;
        break;
      case Orientation::Up:
        angle = 180 - m_ori_angle;
        m_ori_angle = 180;
        m_ori = Orientation::Up;
      default:
        break;
      }
      m_node->rotatelocal('y', angle, m_node->trans);
      
    }

GameObject::GameObject(const float scale,std::string name, int hp, shared_ptr<SceneNode> node): Animator(node), m_scale(scale), m_name(name), m_hp(hp),m_hp_max(hp), m_status(Status::Unready){
    m_pos = vec3(m_node->trans[3]);
    m_target_pos = m_pos;
    m_tmp_pos = m_pos;
}

void GameObject::under_attack(float damage) {
      m_hp -= damage;
      GameWindow::set_ui_hp(this);
      // if first time hp < 0, die and set status
      if (m_hp <= 0 && m_status != Status::Dead){
        m_hp = 0.0f;
        die();
        m_status = Status::Dead;
        GameWindow::game_over();
      }
    }

// move with x,z offset
void GameObject::move(float x, float z){
    DLOG("Object %s move x:%f z:%f", m_name.c_str(), x, z);
    //assert(m_status == Status::Idle);
    m_status = Status::Moving;
    cout << "mpos is " << m_pos << endl;
    if (!Scene::on_boundary(m_pos + vec3(x,0,z))){
         m_target_pos = m_pos +  vec3(x, 0, z);
    }   else {
        m_target_pos = m_pos;
    }
    m_tmp_pos = m_pos;
}

void GameObject::set_pos(const glm::vec3& pos){
      vec3 offset = pos - m_pos;
      m_pos = pos;
      m_node->applyRotTranslTransform(glm::translate(offset));
    }

// ------------- Pikachu logics ------------

void Pikachu::update(){

    if (m_status == Status::Unready) return;

    // update attackUnit
    if (m_attacku != nullptr){
        m_attacku->update(get_curr_time());
        if (m_attacku->get_is_done()){
            delete m_attacku;
            m_attacku = nullptr;
        }
    }

    // if done animation, set status to idle
    if (!get_has_anim() && m_status != Status::Dead){
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
    //assert(m_status == Status::Idle);
    GameObject::move(x, y);
    // do move animation
    Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("pikachu_move");
    assert(ani_ptr != nullptr);
    do_animation(*ani_ptr);
}

void Pikachu::attack(const std::string& name, GameObject* target){
    //assert(m_status == Status::Idle);
    DLOG("Pikachu %s attack enemy %s", m_name.c_str(), target->get_name().c_str());
    // do attack animation and emit attack unit
    m_status = Status::Attacking;
    if (name == "discharge_left"){
         m_attacku = new Discharge(this, target, LEFT);
    }   else if (name == "discharge_right"){
        m_attacku = new Discharge(this, target, RIGHT);
    }   else {
        DLOG("No such attack name! aborted!");
        abort();
    }
   
    return;
}

// Pikachu die effects
void Pikachu::die(){
    m_node->scale(vec3(1, 0.05, 1));
    m_node->translate(vec3(0, -8.0f, 0));
}

Pikachu::~Pikachu(){

}


// ------------- Snorlax logics ------------

void Snorlax::attack(const std::string& name, GameObject* target){
    assert(m_status == Status::Idle);
    DLOG("Snorlax %s attack enemy %s", m_name.c_str(), target->get_name().c_str());
    // add details here
    m_status = Status::Attacking;
    bool decision_bodyslam = Random::Float() >= 0.0f;
    if (decision_bodyslam){
        m_attacku = new BodySlam(this, HumanPlayer::get_instance()->get_GameObject());
    }   else {
        Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("snorlax_meteorite_fall");
        assert(ani_ptr != nullptr);
        do_animation(*ani_ptr);
        int base_num = 10 + 20 * float(m_hp_max - m_hp)/float(m_hp_max);
        generate_meteorite(base_num);
    }
    
    return;
}


void Snorlax::stun(){
    Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("snorlax_bodyslam_stun");
    assert(ani_ptr != nullptr);
    do_animation(*ani_ptr);
    m_status = Status::Stunned;
    m_target_pos = m_pos;
    m_tmp_pos = m_pos;
}

Snorlax::~Snorlax(){
    
}

void Snorlax::die(){
    Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("snorlax_bodyslam_dead");
    assert(ani_ptr != nullptr);
    do_animation(*ani_ptr);
    m_status = Status::Dead;
    m_target_pos = m_pos;
    m_tmp_pos = m_pos;
}

void Snorlax::update(){
    if (m_status == Status::Unready) return;

    // if no animation and dead, return
    if (!get_has_anim() && m_status == Status::Dead){
        return;
    }

    // if no animation and not attacking, set to idle
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