#pragma once
#include "JointNode.hpp"
#include <unordered_map>
#include <iostream>
using namespace std;

class Command {
    // float,float = xangle_offset, yangle_offset
    std::unordered_map<JointNode*, std::pair<float, float>> nodes;

    public:
    Command(std::unordered_map<JointNode*, std::pair<float, float>>&& nodes): nodes(nodes){}
    void undo(){
        for (auto& n : nodes){
            n.first->rotate_joint(-n.second.first, -n.second.second);
        }
    }
    void redo(){
        for (auto& n : nodes){
            //cout << "undo : x " << n.second.first << " y " << n.second.second << endl; 
            n.first->rotate_joint(n.second.first, n.second.second);
        }
    }
};
