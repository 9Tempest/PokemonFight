#pragma
#include "GameObject.hpp"

class Player : public Visitor{
    protected:
        Player(){}
        GameObject* m_obj;
        std::unordered_map<std::string, SceneNode*> m_node_mapping;

        void load_mapping(SceneNode* root_node){
            root_node->accept(*this);
        }

    public:
        void visit(SceneNode* node){
            m_node_mapping[node->m_name] = node;
        }

        virtual ~Player(){
            delete m_obj;
        }
        
        GameObject* get_GameObject(){
            return m_obj;
        }

        void set_GameObject(GameObject* obj){
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
            return m_node_mapping[name];
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