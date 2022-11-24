#ifndef __PARTICLE
#define __PARTICLE



#include <unordered_map>
#include <string>
#include "include.hpp"
#include "timestamp.hpp"
#include <vector>


class GeometryNode;


struct ParticleProps{
    glm::vec3 position;
    glm::vec3 vel;
    float lifetime;
};


const float gravity = 9.8f;

class ParticleSystem{

    private:
        struct Particle{
            glm::vec3 m_position;
            glm::vec3 m_vel;
            glm::vec3 m_rot;
            glm::vec3 m_rot_vel;

            float m_size;
            
            float m_lifetime = 1.0f;
            float m_life_remaining = 0.0f;
            bool m_active = false;

            void update_on(float time_diff, float gravity);

        };

        uint m_pool_idx = 0;
        std::string m_name;
        time_stamp m_curr_ts;
        
    public:
        std::vector<Particle> m_pool;
        std::string get_name() const {return m_name;}
        ParticleSystem(){}
        ParticleSystem(const std::string & name);
        ParticleSystem(const ParticleSystem& other);
        void update();
        
        void Emit(const ParticleProps& pp);     
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

void dirt_flying_effect(float radius, const glm::vec3& pos);

#endif
