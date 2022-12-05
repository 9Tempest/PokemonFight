#pragma once
#include "GameObject.hpp"

// Singleton class that stores the GameObject
class Player : public Visitor{
    protected:
        Player(){}
        GameObject* m_obj = nullptr;
        std::unordered_map<std::string, SceneNode*> m_node_mapping;

        void load_mapping(SceneNode* root_node){
            //std::cout << "load mapping for" <<  m_obj->get_name() << std::endl;
            root_node->accept(*this);
        }

    public:
        void visit(SceneNode* node) override{
            m_node_mapping[node->m_name] = node;
            //std::cout << node->m_name << " maps to " << node << std::endl;
        }

        virtual ~Player(){
            delete m_obj;
        }
        
        GameObject* get_GameObject(){
            return m_obj;
        }

        void set_GameObject(GameObject* obj){
            if (m_obj != nullptr){
                delete m_obj;
            }
            m_obj = obj;
            // clean map
            m_node_mapping.clear();
            // load mapping
            load_mapping(obj->get_rootNode());
        }

        SceneNode* get_root_node(){
            return m_obj->get_rootNode();
        }

        SceneNode* get_node_by_name(const std::string& name){
            if (m_node_mapping.find(name) != m_node_mapping.end())
                return m_node_mapping[name];
            std::cout << "cannot find node " << name << std::endl;
            return nullptr;
        }

};

class AI : public Player{
    protected:
        AI(): Player(){}

    public:
        static AI* get_instance(){
            static AI p;
            return &p;
        }

};

class HumanPlayer : public Player{
    protected:
        HumanPlayer(): Player(){}

    public:
        static HumanPlayer* get_instance(){
            static HumanPlayer p;
            return &p;
        }
};