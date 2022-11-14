#pragma once
#include "GameObject.hpp"

class Command{
    public:
        void virtual execute(GameObject& obj) = 0;
};

class MoveCommand : public Command{
    float m_x;
    float m_y;

    public:
        MoveCommand(float x, float y):m_x(x), m_y(y){}
        void execute(GameObject& obj) override{
            obj.move(m_x, m_y);
        }
};

class AttackCommand : public Command{
    std::string m_attack_name;
    GameObject* m_target;

    public:
        AttackCommand(const std::string& name, GameObject* target) : m_attack_name(name), m_target(target){}
        void execute(GameObject& obj) override{
            obj.attack(m_attack_name, m_target);
        }
};
