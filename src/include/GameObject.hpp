#pragma once
#include <string>
#include <glm/glm.hpp>
#include <memory.h>
#include "Animation.hpp"
#include "particle.hpp"
#include "AttackUnit.hpp"
class SceneNode;
class Command;

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
    //ParticleSystem m_particleSystem;
    std::string m_name;
    
  public:
    glm::vec3 m_pos;
    glm::vec3 m_tmp_pos; // for move interpolation
    glm::vec3 m_target_pos;
     Status m_status = Status::Idle;
  protected:
    int m_hp;
   
    AttackUnit* m_attacku = nullptr;
    friend class AttackUnit;
    
  public:
    virtual ~GameObject(){}
    // GameObject(std::string name, int hp, SceneNode* node):m_name(name), m_hp(hp), m_node(node){
        
    // }
    GameObject(std::string name, int hp, std::shared_ptr<SceneNode> node);
    // for debugging purpose
    Status get_status() const {return m_status;}
    std::string get_name() const {return m_name;}
    glm::vec3 get_pos() const {return m_pos;}
    void set_pos(const glm::vec3& pos);
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
    virtual void attack(const std::string& name, GameObject* target) = 0;
    virtual void under_attack(AttackUnit* attackUnit) = 0;

};

class Pikachu: public GameObject{

  public:
    Pikachu(std::shared_ptr<SceneNode> node): GameObject("Pikachu", 100, node){

    }
    ~Pikachu();
    void update() override;
    void move(float x, float y) override;
    void attack(const std::string& name,  GameObject* target) override;
    void under_attack(AttackUnit* attackUnit) override;

};

class Snorlax: public GameObject{

  float m_radius;

  public:
    float get_radius() const { return m_radius; }
    Snorlax(std::shared_ptr<SceneNode> node): GameObject("Snorlax", 500, node), m_radius(5.0f){

    }
    ~Snorlax();
    void update() override;
    void attack(const std::string& name,  GameObject* target) override;
    void under_attack(AttackUnit* attackUnit) override;


};