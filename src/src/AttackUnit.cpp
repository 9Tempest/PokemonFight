#include "AttackUnit.hpp"
#include "GameObject.hpp"
#include <include.hpp>
#include "GameWindow.hpp"
#include "PlayerAI.hpp"
#include "polyroots.hpp"
#include "debug.hpp"
using namespace std;
using namespace glm;

float SKY_HEIGHT = 70.0f;

BodySlam::BodySlam(GameObject*attacker, GameObject*attackee)
    :AttackUnit("BodySlam", 50.0f, attacker, attackee), m_radius_square(pow2(attacker->get_radius())){
        Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("snorlax_bodyslam_up");
        assert(ani_ptr != nullptr);
        assert(m_attacker != nullptr);
        m_attacker->do_animation(*ani_ptr);
        m_attacker->set_target_pos( vec3(m_attacker->get_pos().x, SKY_HEIGHT, m_attacker->get_pos().z));
        
    }

void BodySlam::phase2(){
    vec3 target_pos = vec3(m_target->get_pos().x, SKY_HEIGHT, m_target->get_pos().z);
    vec3 target_pos_ground = vec3(m_target->get_pos().x, -3, m_target->get_pos().z);
    m_attacker->set_pos(target_pos);
    m_attacker->set_target_pos(target_pos_ground);
    Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("snorlax_bodyslam_down");
    assert(ani_ptr != nullptr);
    m_attacker->do_animation(*ani_ptr);
    m_attacker->set_status(Status::Attacking);
}

bool BodySlam::hit(){
    return in_circle(m_pos, m_target->get_pos(), m_attacker->get_radius());
}

void BodySlam::update(const time_stamp& ts){
    float time_diff = get_time_diff(m_ts, ts);

    if (!phase1 && is_touch_sky){
        phase1 = true;
        phase2();
    }

    if (phase1 && m_attacker->get_pos().y <= -2.0f){
        dirt_flying_effect(m_attacker->get_radius(), m_attacker->get_pos());
        GameWindow::cameraShake(2.0f);
        m_pos = m_attacker->get_pos();
        if (hit()){
            m_target->under_attack(m_damage);
        }
        m_attacker->stun();
        m_done = true;
        #ifdef UNMUTED
        m_sound = SoundEngine::play3D(SOUND_SLAM, m_attacker->get_pos(), false, 1.5f);
        assert(m_sound != nullptr);
        #else
        #endif
    }
}


Discharge::Discharge(GameObject*attacker, GameObject*attackee, int ori):AttackUnit("Discharge", 1.0f, attacker, attackee), m_ori(ori){
       Animation* ani_ptr = nullptr;
        if (ori == RIGHT){
            ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("pikachu_attack_right");
        }   else if (ori == LEFT){
            ani_ptr = AnimationLoader::get_instance()->get_animation_by_name("pikachu_attack_left");
        }   else {
            DLOG("No such orientation in discharge");
            abort();
        }
        
        assert(ani_ptr != nullptr);
        assert(m_attacker != nullptr);
        attacker->do_animation(*ani_ptr);
        #ifdef UNMUTED
        m_sound = SoundEngine::play3D(SOUND_LIGHTNING, m_attacker->get_pos(), false, 1.5f);
        assert(m_sound != nullptr);
        #else
        #endif
    }


bool Discharge::hit(){
            // a sphere ball intersection problem
            double a = 1;
            vec3 target_pos = m_target->get_pos();
            double b = 2 * dot(m_dir, (m_pos -target_pos));
            double c = dot(m_pos - target_pos, m_pos-target_pos)-pow(m_target->get_radius(), 2);
            double ts[2];
            size_t num_roots = quadraticRoots(a,b,c,ts);
            if (num_roots == 0){
                return false;
            }   else if (num_roots == 1){
                if (ts[0] > -10) return true;
            }   else {
                double t = glm::min(ts[0], ts[1]);
                if (t > -10) return true;
            }

            return false;
        }

void Discharge::update(const time_stamp& ts){
    float time_diff = get_time_diff(m_ts, ts);
    
    remaining_particle_effect_time -= time_diff;
    m_ts = ts;
    if (remaining_particle_effect_time <= 0.0f){
        SceneNode* hand = nullptr;
        // get discharge position
        if (m_ori == RIGHT){
             hand = HumanPlayer::get_instance()->get_node_by_name("elbowr");
        }   else {
            hand = HumanPlayer::get_instance()->get_node_by_name("elbowl");
        }
        assert(hand != nullptr);
        vec3 dir = m_attacker->get_dir();
        vec3 pos = (vec3)(hand->trans[3]*m_attacker->get_scale())  + 2*dir;
        m_pos = pos;
        m_dir = dir;
        // determine if hit or not
        if (hit()){
            //cout << "hit target! at pos " << m_target->get_pos() << endl;
            GameWindow::cameraShake(0.1f, 0.5f);
            m_target->under_attack(m_damage);
        }
        lightning_effect(pos, dir);
        remaining_particle_effect_time = 0.095f;
    }

    if (m_attacker->get_status() != Status::Attacking){
        m_done = true;
        #ifdef UNMUTED
        m_sound->stop();
        #else
        #endif
    }
}

bool in_circle(const glm::vec3& m_pos, const glm::vec3& target_pos, double radius){
    double x = target_pos.x - m_pos.x;
    double z = target_pos.z - m_pos.z;
    return (glm::pow2(x) + glm::pow2(z) < radius*radius);
}
