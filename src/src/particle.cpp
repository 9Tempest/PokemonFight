#include "particle.hpp"
#include "debug.hpp"
#include "random.hpp"
#include "GeometryNode.hpp"

using namespace std;
using namespace glm;

std::unordered_map<std::string, GeometryNode*> ParticleAssets::assets_mesh = std::unordered_map<std::string, GeometryNode*>();
std::unordered_map<std::string, ParticleSystem> ParticleAssets::assets_systems = std::unordered_map<std::string, ParticleSystem>();

void ParticleAssets::add_asset(GeometryNode* mesh){
    assets_mesh[mesh->m_name] = mesh;
    assets_systems[mesh->m_name] = ParticleSystem(mesh->m_name);
}

ParticleSystem* ParticleAssets::fetch_system(const std::string& name){
    if (assets_systems.find(name) == assets_systems.end()) return nullptr;
    return &assets_systems[name];
}

GeometryNode* ParticleAssets::fetch_mesh(const std::string& name){
    if (assets_mesh.find(name) == assets_mesh.end()) return nullptr;
    return assets_mesh[name];
}



ParticleSystem::ParticleSystem(const string&name) : m_name(name){
    m_curr_ts = get_curr_time();
    m_pool.resize(1000);
}

ParticleSystem::ParticleSystem(const ParticleSystem& other) 
    :m_pool_idx(other.m_pool_idx), m_name(other.m_name),m_curr_ts(other.m_curr_ts), m_pool(other.m_pool){

    }


void ParticleSystem::Particle::update_on(float time_diff, float gravity){
    assert(time_diff >= 0);
    if (m_position.y > GROUND || m_vel.y >= 0){
        m_position += time_diff * m_vel;
        m_vel.y -= time_diff * gravity;
        m_rot += time_diff * m_rot_vel;
    }
    
   
    if (m_lifetime <= 0){
        m_active = false;
        m_lifetime = 0;
    }   // if

    m_lifetime -= time_diff;
}

void ParticleSystem::update(){
    time_stamp curr_time_update = get_curr_time();
    float time_diff = get_time_diff(m_curr_ts, curr_time_update);
    //cout << "time diff is " << time_diff << endl;
    for (auto& p : m_pool){
        if (p.m_active){
            p.update_on(time_diff, GRAVITY);
            // .. update
        }   // if
    }   // for

    m_curr_ts = curr_time_update;
}

const float INIT_ROT_ANGLE = 60.0f;

void ParticleSystem::Emit(const ParticleProps& pp){
    // fetch next available particle
    Particle& particle = m_pool[m_pool_idx];
    particle.m_active = true;
    particle.m_position = pp.position;
    particle.m_lifetime = pp.lifetime;
    // size
    particle.m_size = Random::FloatPositive() * 2;
    // velocity
    particle.m_vel = pp.vel;
    particle.m_vel.x += Random::Float();
    particle.m_vel.z += Random::Float();

    // rotation vel
    particle.m_rot_vel.x = float(INIT_ROT_ANGLE) * Random::Float();
    particle.m_rot_vel.y = float(INIT_ROT_ANGLE) * Random::Float();
    particle.m_rot_vel.z = float(INIT_ROT_ANGLE) * Random::Float();

    m_pool_idx = ++m_pool_idx % m_pool.size();

}
const float PI = 3.141592653589793238;

void dirt_flying_effect(float radius, const glm::vec3& pos){
    int num_pts = Random::Int(20,40);
    radius *= 1.5;
    float k = 2 * PI / float(num_pts);
    for(int i_phi = 0; i_phi < num_pts; ++i_phi){
        float const phi = i_phi * k;
        float positionX = radius * cos( phi );
        float positionZ = radius * sin( phi );
        vec3 ppos = vec3(positionX+pos.x, GROUND, positionZ+pos.z);
        vec3 vel = normalize(ppos - pos) + vec3(0, 8, 0);
        ParticleProps pps{ppos, vel, 2.0f};
        ParticleAssets::fetch_system("dirt")->Emit(pps);
    }
}
