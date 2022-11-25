#pragma once
#include <string>
#include <glm/glm.hpp>
#include "timestamp.hpp"

class GameObject;

struct AttackUnitArgs{
    float arg1;
    float arg2;
    float arg3;
};

class AttackUnit{
  protected:
    std::string m_name;
    float m_damage;
    bool m_done = false;
    glm::vec4 m_pos;
    GameObject* m_attacker = nullptr;
    GameObject* m_target = nullptr;
    AttackUnitArgs m_args;
    time_stamp m_ts;
  public:
        virtual ~AttackUnit(){
        }

        AttackUnit(const std::string& name, float damage, GameObject*attacker, GameObject*attackee):
            m_name(name), m_damage(damage), m_attacker(attacker), m_target(attackee){
                m_ts = get_curr_time();
            }
        
        void set_position_target_args(const glm::vec4& pos, GameObject* target, const AttackUnitArgs& args){
            m_pos = pos;
            m_target = target;
            m_args = args;
        }
        bool get_is_done() const{return m_done;}

        virtual bool hit() = 0;

        virtual void update(const time_stamp& curr_ts){}
};

class Discharge : public AttackUnit{
    float remaining_particle_effect_time = 0.095f;
    public:
        Discharge(GameObject*attacker, GameObject*attackee);
        bool hit(){

            return false;
        }
        void update(const time_stamp& curr_ts) override;

};

class BodySlam : public AttackUnit{
    bool phase1 = false;
    
        void phase2();
    public:
        bool is_touch_sky = false;
        BodySlam(GameObject*attacker, GameObject*attackee);
        bool hit(){

            return false;
        }

        void update(const time_stamp& curr_ts) override;

};


