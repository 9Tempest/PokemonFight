#pragma once

#include "SceneNode.hpp"
#include <unordered_map>
#include "GeometryNode.hpp"
#include <iostream>
#include <glm/gtx/io.hpp>
#define CONVERT_ID2COLOR( i ) glm::vec3(((i & 0x000000FF) >> 0)/255.0f, ((i & 0x0000FF00) >> 8)/255.0f, ((i & 0x00FF0000) >> 16)/255.0f)



class Picker : public Visitor{
    std::unordered_map<int, GeometryNode*> id2node;

    GeometryNode* pick(unsigned char color[4]){
        int id = convert_color2id(color);
        GeometryNode* res;
        if (id2node.find(id) != id2node.end()){
            res = id2node[id];
        }   else {
            res = nullptr;
        }
        return res;
    }
    public:
        bool is_picking;
        Picker(SceneNode* root):is_picking(false){
            root->accept(*this);
        }
        static int convert_color2id(unsigned char color[4]) {
            return color[0]+color[1]*256 +color[2]*256*256;
        }

        GeometryNode* pick(double xPos, double yPos){
            unsigned char color[4];
            // all the fragments completely rasterized.
            // Wait until all the pending drawing commands are really done.
            // Ultra-mega-over slow !
            // There are usually a long time between glDrawElements() and
            // all the fragments completely rasterized.
            glFlush();
            glFinish();
            // memory alignment
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glReadPixels((GLint)xPos, (GLint)yPos,1,1, GL_RGBA, GL_UNSIGNED_BYTE, color);
            return pick(color);
        }



        virtual void visit(SceneNode* node){
            if (node->m_nodeType == NodeType::GeometryNode){
                GeometryNode* gnode = (GeometryNode*)node;
                // update original color
                gnode->original_material = gnode->material;
                // compute uniq color
                gnode->uniq_color = CONVERT_ID2COLOR(gnode->m_nodeId);
                // insert into map
                id2node[node->m_nodeId] = gnode;
                //std::cout << "uniq color " << ((GeometryNode*)node)->uniq_color << std::endl;
            }
        }
};
