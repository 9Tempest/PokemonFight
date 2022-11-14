#pragma once
#include <string>
#include <glm/glm.hpp>

class Particles;
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
    Particles* m_particle;
    glm::vec4 m_pos;
    GameObject* m_target = nullptr;
    AttackUnitArgs m_args;
  public:
        virtual ~AttackUnit(){
            delete m_particle;
        }

        AttackUnit(const std::string& name, float damage, Particles* particle):
            m_name(name), m_damage(damage), m_particle(particle){}
        
        void set_position_target_args(const glm::vec4& pos, GameObject* target, const AttackUnitArgs& args){
            m_pos = pos;
            m_target = target;
            m_args = args;
        }

        virtual bool hit() = 0;
};

class Discharge : public AttackUnit{

    public:
        Discharge():AttackUnit("Discharge", 10.0f, nullptr /* null for now*/){

        }
        bool hit(){

            return false;
        }

};

class BodySlam : public AttackUnit{

    public:
        BodySlam():AttackUnit("BodySlam", 30.0f, nullptr /* null for now*/){

        }
        bool hit(){

            return false;
        }

};


