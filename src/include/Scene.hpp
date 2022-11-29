#pragma once
#include <memory.h>
#include "SceneNode.hpp"
class Scene{
    std::shared_ptr<SceneNode> m_node;
    static constexpr float XMIN = -50.0f;
    static constexpr float XMAX = 50.0f;
    static constexpr float ZMAX = 5.0f;
    static constexpr float ZMIN = -50.0f;
    public:
        Scene(std::shared_ptr<SceneNode> node):m_node(node) {
            
        }

        void render(Visitor& v){
            m_node->accept(v);
        }


        static bool on_boundary(const glm::vec3 &pos){
            return pos.x < XMIN || pos.x > XMAX || pos.z <ZMIN || pos.z > ZMAX;
        }
};