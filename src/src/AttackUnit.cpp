#include "AttackUnit.hpp"
#include "GameObject.hpp"
#include <include.hpp>
#include "GameWindow.hpp"
#include "PlayerAI.hpp"
using namespace std;
using namespace glm;

float SKY_HEIGHT = 100.0f;

BodySlam::BodySlam(GameObject*attacker, GameObject*attackee)
    :AttackUnit("BodySlam", 30.0f, attacker, attackee){
        Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("snorlax_bodyslam_up");
        assert(ani_ptr != nullptr);
        assert(m_attacker != nullptr);
        m_attacker->do_animation(*ani_ptr);
        m_attacker->set_target_pos( vec3(m_attacker->get_pos().x, SKY_HEIGHT, m_attacker->get_pos().z));
        
    }

void BodySlam::phase2(){
    vec3 target_pos = vec3(m_target->get_pos().x, SKY_HEIGHT, m_target->get_pos().z);
    m_attacker->set_pos(target_pos);
    m_attacker->set_target_pos(m_target->get_pos());
    Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("snorlax_bodyslam_down");
    assert(ani_ptr != nullptr);
    m_attacker->do_animation(*ani_ptr);
    m_attacker->set_status(Status::Attacking);
}


void BodySlam::update(const time_stamp& ts){
    float time_diff = get_time_diff(m_ts, ts);

    if (!phase1 && is_touch_sky){
        phase1 = true;
        phase2();
    }

    if (phase1 && m_attacker->get_pos().y <= 0){
        dirt_flying_effect(m_attacker->get_radius(), m_attacker->get_pos());
        GameWindow::cameraShake(2.0f);
        if (hit()){
            m_target->under_attack(this);
        }
        m_done = true;
    }
}


Discharge::Discharge(GameObject*attacker, GameObject*attackee):AttackUnit("Discharge", 10.0f, attacker, attackee){
        Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("pikachu_attack");
        assert(ani_ptr != nullptr);
        assert(m_attacker != nullptr);
        attacker->do_animation(*ani_ptr);
    }

void Discharge::update(const time_stamp& ts){
    float time_diff = get_time_diff(m_ts, ts);
    remaining_particle_effect_time -= time_diff;
    m_ts = ts;
    if (remaining_particle_effect_time <= 0.0f){
        SceneNode* hand = HumanPlayer::get_instance()->get_node_by_name("elbowr");
        assert(hand != nullptr);
        vec3 dir = m_attacker->get_dir();
        vec3 pos = (vec3)hand->trans[3] + 2*dir;
        lightning_effect(pos, dir);
        remaining_particle_effect_time = 0.095f;
    }

    if (m_attacker->get_status() == Status::Idle){
        m_done = true;
        cout << "Done discharge !" << endl;
    }
}
