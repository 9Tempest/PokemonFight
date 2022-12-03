#pragma once
#include <string>
#include <glm/glm.hpp>
#include <memory.h>
#include "Animation.hpp"
#include "particle.hpp"
#include "AttackUnit.hpp"
class SceneNode;
class Command;

// status of game objects
enum class Status{
    Moving,
    Attacking,
    Idle,
    Stunned,
    UnderAttack,
    Dead
};

// orientation of pikachu
enum class Orientation{
  Left,
  Right,
  Up,
  Down
};

const int HP_MAX_SNORLAX = 400;
const int HP_MAX_PIKACHU = 100;

class GameObject : public Animator{
  
  protected:
    //ParticleSystem m_particleSystem;
    const float m_scale;
    std::string m_name;
    glm::vec3 m_pos;
    glm::vec3 m_tmp_pos; // for move interpolation
    glm::vec3 m_target_pos;
    Status m_status = Status::Idle;
    Orientation m_ori = Orientation::Down;
    int m_hp;
    int m_hp_max;
    float m_ori_angle = 0.0f;
   
    AttackUnit* m_attacku = nullptr;
    friend class AttackUnit;
    
  public:
    virtual ~GameObject(){}
    // GameObject(std::string name, int hp, SceneNode* node):m_name(name), m_hp(hp), m_node(node){
        
    // }
    GameObject(const float scale, std::string name, int hp, std::shared_ptr<SceneNode> node);
    // for debugging purpose
    const float get_scale() const{return m_scale;}
    Status get_status() const {return m_status;}
    void set_status(Status s){m_status = s;}
    std::string get_name() const {return m_name;}
    glm::vec3 get_pos() const {return m_pos;}
    void set_pos(const glm::vec3& pos);
    void set_target_pos(const glm::vec3& pos){
      m_target_pos = pos;
      m_tmp_pos = m_pos;
    }
    void set_orientation(Orientation o);
    glm::vec3 get_dir() const{
      switch (m_ori)
      {
      case Orientation::Down:
        return glm::vec3(0,0,1);
        break;
      case Orientation::Right:
        return glm::vec3(1,0,0);
      case Orientation::Up:
        return glm::vec3(0,0,-1);
        break;
      case Orientation::Left:
        return glm::vec3(-1,0,0);
      default:
        abort();
      }
    }
    int get_hp() const {return m_hp;}
    int get_hp_max() const {return m_hp_max;}
    virtual float get_radius() const{return 0;}
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
    virtual void die() = 0;

    virtual void under_attack(float damage);
    virtual void stun(){}
    
};

class Pikachu: public GameObject{

  public:
    Pikachu(std::shared_ptr<SceneNode> node): GameObject(1.0f, "Pikachu", HP_MAX_PIKACHU, node){

    }
    ~Pikachu();
    void update() override;
    void move(float x, float y) override;
    void attack(const std::string& name,  GameObject* target) override;
    void die() override;

};

class Snorlax: public GameObject{

  float m_radius;

  public:
    float get_radius() const override { return m_radius; }
    Snorlax(std::shared_ptr<SceneNode> node): GameObject(1.0f, "Snorlax", HP_MAX_SNORLAX, node), m_radius(8.0f){

    }
    ~Snorlax();
    void update() override;
    void attack(const std::string& name,  GameObject* target) override;
    void die() override;
    void stun() override;

};