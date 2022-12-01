#ifndef __PARTICLE
#define __PARTICLE



#include <unordered_map>
#include <string>
#include "include.hpp"
#include "timestamp.hpp"
#include <vector>

class GameObject;
class GeometryNode;


struct ParticleProps{
    glm::vec3 position;
    glm::vec3 vel;
    float lifetime;
    glm::vec3 m_rot = glm::vec3(0,0,0);
    float size = 0.0f;
    float gravity = GRAVITY;
};




class ParticleSystem{

    public:
        struct Particle{
            glm::vec3 m_position;
            glm::vec3 m_vel;
            glm::vec3 m_rot;
            glm::vec3 m_rot_vel;

            float m_size = 0.0f;
            float gravity = GRAVITY;
            float m_lifetime = 1.0f;
            float m_life_remaining = 0.0f;
            bool m_active = false;
            typedef void (*DestoryFunc)(ParticleSystem::Particle&, GameObject*);
            bool is_hit();
            void update_on(float time_diff, float gravity, DestoryFunc func, bool destory_when_touch_ground = false);

        };
    private:
        uint m_pool_idx = 0;
        std::string m_name;
        time_stamp m_curr_ts;
        
    public:
        std::vector<Particle> m_pool;
        std::string get_name() const {return m_name;}
        ParticleSystem(){}
        ParticleSystem(const std::string & name);
        ParticleSystem(const ParticleSystem& other);
        void update(bool gravity = true, Particle::DestoryFunc func = nullptr, bool destory_when_touch_ground = false);
        void clean(){
            m_pool.clear();
            m_pool.resize(1000);
        }
        void Emit(const ParticleProps& pp, bool is_random = true);     
};

class ParticleAssets{
    private:
        // particle name -> mesh id
        static std::unordered_map<std::string, GeometryNode*> assets_mesh;
        static std::unordered_map<std::string, ParticleSystem> assets_systems;
    public:
        static void add_asset(GeometryNode* mesh);
        static ParticleSystem* fetch_system(const std::string& name);
        static GeometryNode* fetch_mesh(const std::string& name);
};

void dirt_flying_effect(float radius, const glm::vec3& pos, int base_num = 30);

void lightning_effect(const glm::vec3& pos, const glm::vec3& dir);

void generate_meteorite();


typedef ParticleSystem::Particle::DestoryFunc DestoryFunc;

void meteorite_destroy(ParticleSystem::Particle&, GameObject*);
#endif
