#include "Scene.hpp"
#include "SceneNode.hpp"


SceneNode* Scene::grass_node = nullptr;

void Scene::set_grass_node(){
    assert(m_node->children.front()->m_name == "grasses");
    grass_node =  m_node->children.front();
}