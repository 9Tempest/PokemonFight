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

void ParticleSystem::update(bool gravity){
    time_stamp curr_time_update = get_curr_time();
    float time_diff = get_time_diff(m_curr_ts, curr_time_update);
    //cout << "time diff is " << time_diff << endl;
    for (auto& p : m_pool){
        if (p.m_active){
            p.update_on(time_diff, gravity?GRAVITY:0.0f);
            // .. update
        }   // if
    }   // for

    m_curr_ts = curr_time_update;
}

const float INIT_ROT_ANGLE = 60.0f;

void ParticleSystem::Emit(const ParticleProps& pp, bool is_random){
    // fetch next available particle
    Particle& particle = m_pool[m_pool_idx];
    particle.m_active = true;
    particle.m_position = pp.position;
    particle.m_lifetime = pp.lifetime;

    // velocity
    particle.m_vel = pp.vel;

    // rot
    particle.m_rot = pp.m_rot;
    particle.m_size = pp.size;

    if (is_random){
        // size
        particle.m_size = Random::FloatPositive() * 2;
        particle.m_vel.x += Random::Float();
        particle.m_vel.z += Random::Float();

        // rotation vel
        particle.m_rot_vel.x = float(INIT_ROT_ANGLE) * Random::Float();
        particle.m_rot_vel.y = float(INIT_ROT_ANGLE) * Random::Float();
        particle.m_rot_vel.z = float(INIT_ROT_ANGLE) * Random::Float();
    }
    m_pool_idx = ++m_pool_idx % m_pool.size();

}


void dirt_flying_effect(float radius, const glm::vec3& pos){
    int num_pts = Random::Int(30,50);
    radius *= 1.5;
    float k = 2 * PI / float(num_pts);
    for(int i_phi = 0; i_phi < num_pts; ++i_phi){
        float const phi = i_phi * k;
        float positionX = radius * cos( phi );
        float positionZ = radius * sin( phi );
        vec3 ppos = vec3(positionX+pos.x, GROUND, positionZ+pos.z);
        vec3 vel = 2 * normalize(ppos - pos) + vec3(0, 10, 0);
        ParticleProps pps{ppos, vel, 3.0f};
        ParticleAssets::fetch_system("dirt")->Emit(pps);
    }
}

const int NUM_LIGHTNING_PTS = 10;

void lightning_effect(const glm::vec3& pos, const glm::vec3& dir){
    int num_pts = NUM_LIGHTNING_PTS;
    float rot_angle = 0.0f;
    if (dir.z != 0.0f){
        rot_angle -= 90;
    }
    
    for (int i = 0; i < num_pts; i++){
        vec3 ppos = pos + i * dir;
        vec3 rot = vec3(INIT_ROT_ANGLE/2 * Random::Float(), INIT_ROT_ANGLE/2* Random::Float(), INIT_ROT_ANGLE/2* Random::Float());
        rot.y = rot_angle;
        //cout << "rot angle is " << rot << endl;
        ParticleProps pps{ppos, vec3(0), 0.1f, rot};
        ParticleAssets::fetch_system("lightning")->Emit(pps, false);
    }

    for (int i = 0; i < num_pts; i++){
        vec3 ppos = pos + i * dir;
        ppos.y += Random::FloatPositive();
        vec3 rot = vec3(INIT_ROT_ANGLE * Random::FloatPositive(), INIT_ROT_ANGLE* Random::FloatPositive(), INIT_ROT_ANGLE* Random::FloatPositive());
        rot.y = rot_angle;
        ParticleProps pps{ppos, vec3(0), 0.1f, rot, 0.5};
        ParticleAssets::fetch_system("lightning")->Emit(pps, false);

        rot = vec3(-INIT_ROT_ANGLE * Random::FloatPositive(), -INIT_ROT_ANGLE* Random::FloatPositive(), -INIT_ROT_ANGLE* Random::FloatPositive());
        rot.y = rot_angle;
        ppos = pos + i * dir;
        ppos.y -=  Random::FloatPositive();
        pps = ParticleProps{ppos, vec3(0), 0.1f, rot, 0.5};
        ParticleAssets::fetch_system("lightning")->Emit(pps, false);
    }

    for (int i = 0; i < num_pts*10; i++){
        vec3 ppos = pos + i * dir/9;
        ppos += vec3(Random::FloatPositive() * 2,Random::FloatPositive() * 2, Random::FloatPositive() * 2);
        vec3 rot = vec3(INIT_ROT_ANGLE * Random::FloatPositive(), INIT_ROT_ANGLE* Random::FloatPositive(), INIT_ROT_ANGLE* Random::FloatPositive());
        rot.y = rot_angle;
        ParticleProps pps{ppos, vec3(0), 0.1f, rot};
        ParticleAssets::fetch_system("electornics")->Emit(pps, false);

        rot = vec3(-INIT_ROT_ANGLE * Random::FloatPositive(), -INIT_ROT_ANGLE* Random::FloatPositive(), -INIT_ROT_ANGLE* Random::FloatPositive());
        rot.y = rot_angle;
        ppos = pos + i * dir/9;
        ppos -= vec3(Random::FloatPositive() * 2,Random::FloatPositive() * 2, Random::FloatPositive() * 2);
        pps = ParticleProps{ppos, vec3(0), 0.1f, rot};
        ParticleAssets::fetch_system("electornics")->Emit(pps, false);
    }
}
