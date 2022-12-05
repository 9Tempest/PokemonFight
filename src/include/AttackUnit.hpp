#pragma once
#include <string>
#include <glm/glm.hpp>
#include "timestamp.hpp"
#include "sound.hpp"

#define LEFT 0
#define RIGHT 1

class GameObject;


class AttackUnit{
  protected:
    std::string m_name;
    float m_damage;
    bool m_done = false;
    glm::vec3 m_pos;
    GameObject* m_attacker = nullptr;
    GameObject* m_target = nullptr;
    time_stamp m_ts;
    irrklang::ISound* m_sound = nullptr;
  public:
        virtual ~AttackUnit(){
        }

        AttackUnit(const std::string& name, float damage, GameObject*attacker, GameObject*attackee):
            m_name(name), m_damage(damage), m_attacker(attacker), m_target(attackee){
                m_ts = get_curr_time();
            }
        
        bool get_is_done() const{return m_done;}

        virtual bool hit() = 0;

        virtual void update(const time_stamp& curr_ts){}

        float get_damage() const {return m_damage;}
};

class Discharge : public AttackUnit{
    glm::vec3 m_dir;
    float remaining_particle_effect_time = 0.095f;
    int m_ori;
    public:
        Discharge(GameObject*attacker, GameObject*attackee, int ori);
        bool hit() override;
        void update(const time_stamp& curr_ts) override;

};

class BodySlam : public AttackUnit{
    bool phase1 = false;
    double m_radius_square;
    void phase2();
    public:
        bool is_touch_sky = false;
        BodySlam(GameObject*attacker, GameObject*attackee);
        bool hit() override;

        void update(const time_stamp& curr_ts) override;

};


bool in_circle(const glm::vec3& m_pos, const glm::vec3& target_pos, double radius);

