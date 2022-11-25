#include "AttackUnit.hpp"
#include "GameObject.hpp"
#include <include.hpp>

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
        m_ts = get_curr_time();
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
        if (hit()){
            m_target->under_attack(this);
        }
        m_done = true;
    }
}