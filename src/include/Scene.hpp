#pragma once
#include <memory.h>
#include "SceneNode.hpp"
class Scene{
    std::shared_ptr<SceneNode> m_node;
    public:
        Scene(std::shared_ptr<SceneNode> node):m_node(node) {
            
        }

        void render(Visitor& v){
            m_node->accept(v);
        }
};