#pragma once
#include <string>
#include <glm/glm.hpp>
#include <memory.h>
#include "Animation.hpp"

class SceneNode;
class Command;
class AttackUnit;

enum class Status{
    Moving,
    Attacking,
    Idle,
    Stunned,
    UnderAttack,
    Dead
};

class GameObject : public Animator{
  protected:
    std::string m_name;
    glm::vec4 m_pos;
    int m_hp;
    Status m_status = Status::Idle;

  public:
    virtual ~GameObject(){}
    // GameObject(std::string name, int hp, SceneNode* node):m_name(name), m_hp(hp), m_node(node){
        
    // }
    GameObject(std::string name, int hp, std::shared_ptr<SceneNode> node);
    // for debugging purpose
    std::string get_name() const {return m_name;}
    glm::vec4 get_pos() const {return m_pos;}
    int get_hp() const {return m_hp;}
    SceneNode* get_rootNode() const {return m_node.get();}

    virtual void move(float x, float y);
    /*
      main logic for object to attack another object:
        attackUnit = obj1->attack("discharge", obj2)
        obj1->status = Attacking
        obj1->animationBuffer <- obj1->getAttackAnims()

        in appLogic(): 
          obj1->update() (attackUnit->update())
          if (ts > obj1->animationBuffer[last].ts):
            obj1->status = Idle
            if (attackUnit.hit(attackUnit.target)):
              attackUnit.target.under_attack(attackUnit): if idle, -hp; status = underAttack; add animation 
              delete attackUnit;

    */
    virtual AttackUnit* attack(const std::string& name, GameObject* target) = 0;
    virtual void under_attack(AttackUnit* attackUnit) = 0;

};

class Pikachu: public GameObject{

  public:
    Pikachu(std::shared_ptr<SceneNode> node): GameObject("Pikachu", 100, node){

    }
    ~Pikachu();

    AttackUnit* attack(const std::string& name,  GameObject* target) override;
    void under_attack(AttackUnit* attackUnit) override;

};

class Snorlax: public GameObject{

  public:
    Snorlax(std::shared_ptr<SceneNode> node): GameObject("Snorlax", 500, node){

    }
    ~Snorlax();

    AttackUnit* attack(const std::string& name,  GameObject* target) override;
    void under_attack(AttackUnit* attackUnit) override;

};