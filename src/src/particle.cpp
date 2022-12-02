#include "particle.hpp"
#include "debug.hpp"
#include "random.hpp"
#include "GeometryNode.hpp"
#include "GameObject.hpp"
#include "PlayerAI.hpp"
#include "GameWindow.hpp"
#include "AttackUnit.hpp"
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

static inline bool is_hit(ParticleSystem::Particle& p, GameObject* o){
    bool res = in_circle(p.m_position, o->get_pos(), p.m_size*2);
    return res;
}

float const DAMAGE_METE = 10.0f;

void meteorite_destroy(ParticleSystem::Particle& p, GameObject* o){
    GameWindow::cameraShake(1.0f, 1.0f);
    dirt_flying_effect(p.m_size, p.m_position, 10);
    if (is_hit(p, o)){
        o->under_attack(DAMAGE_METE);
    }
    #ifdef UNMUTED
    SoundEngine::play3D(SOUND_SLAM, p.m_position, false, 1.0f);
    #endif

}


void ParticleSystem::Particle::update_on(float time_diff, float gravity, Particle::DestoryFunc func, bool destory_when_touch_ground){
    assert(time_diff >= 0);
    if (m_position.y > GROUND || m_vel.y >= 0){
        m_position += time_diff * m_vel;
        m_vel.y -= time_diff * gravity;
        m_rot += time_diff * m_rot_vel;
       
    }   else if (destory_when_touch_ground){
        if (func!=nullptr){
            func(*this, HumanPlayer::get_instance()->get_GameObject());
        }
        m_active = false;
    }
    
    if (m_lifetime <= 0){
        m_active = false;
        m_lifetime = 0;
    }   // if

    m_lifetime -= time_diff;
}

void ParticleSystem::update(bool gravity, Particle::DestoryFunc func, bool destory_when_touch_ground){
    time_stamp curr_time_update = get_curr_time();
    float time_diff = get_time_diff(m_curr_ts, curr_time_update);
    //cout << "time diff is " << time_diff << endl;
    for (auto& p : m_pool){
        if (p.m_active){
            p.update_on(time_diff, gravity?p.gravity:0.0f, func, destory_when_touch_ground);
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
    particle.gravity = pp.gravity;

    if (is_random){
        // size
        particle.m_size+= Random::FloatPositive() * 2;

        // vel
        particle.m_vel.x += Random::Float();
        particle.m_vel.z += Random::Float();

        // rotation vel
        particle.m_rot_vel.x = float(INIT_ROT_ANGLE) * Random::Float();
        particle.m_rot_vel.y = float(INIT_ROT_ANGLE) * Random::Float();
        particle.m_rot_vel.z = float(INIT_ROT_ANGLE) * Random::Float();
    }
    m_pool_idx = ++m_pool_idx % m_pool.size();

}

void generate_meteorite(){
    GameWindow::cameraShake(10.0f, 0.5f);
    int num = Random::Int(10,20);
    ParticleProps pps;
    for (int i = 0; i < num; i++){
        float positionX = Random::Float() * 50;
        float positionZ = -Random::FloatPositive() * 50;
        float y_var = Random::Float() * 100;
        vec3 ppos = vec3(positionX, 150.0f + y_var, positionZ);
        pps = {ppos, vec3(0,0,0), 10.0f, vec3(0,0,0), 2.0f, 4.0f};
        ParticleAssets::fetch_system("meteorite")->Emit(pps);
    }
    // emit one mete on top of pikachu
    vec3 target_pos =  HumanPlayer::get_instance()->get_GameObject()->get_pos();
    pps.position = target_pos;
    pps.position.y = 100.0f;
    ParticleAssets::fetch_system("meteorite")->Emit(pps, true);
}

void dirt_flying_effect(float radius, const glm::vec3& pos, int base_num){
    int num_pts = Random::Int(base_num,int(base_num*1.5f));
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
        ParticleProps pps{ppos, vec3(0), 0.1f, rot, 1.0f};
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
        ParticleProps pps{ppos, vec3(0), 0.1f, rot, 1.0f};
        ParticleAssets::fetch_system("electornics")->Emit(pps, false);

        rot = vec3(-INIT_ROT_ANGLE * Random::FloatPositive(), -INIT_ROT_ANGLE* Random::FloatPositive(), -INIT_ROT_ANGLE* Random::FloatPositive());
        rot.y = rot_angle;
        ppos = pos + i * dir/9;
        ppos -= vec3(Random::FloatPositive() * 2,Random::FloatPositive() * 2, Random::FloatPositive() * 2);
        pps = ParticleProps{ppos, vec3(0), 0.1f, rot , 1.0f};
        ParticleAssets::fetch_system("electornics")->Emit(pps, false);
    }
}
