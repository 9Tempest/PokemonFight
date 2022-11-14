#pragma
#include "GameObject.hpp"

class Player{
    private:
        Player(){}
        GameObject* m_obj;
    public:
        ~Player(){
            delete m_obj;
        }
        static Player* get_instance(){
            static Player p;
            return &p;
        }
        GameObject* get_GameObject(){
            return m_obj;
        }

        void set_GameObject(GameObject* obj){
            m_obj = obj;
        }
        SceneNode* get_root_node(){
            return m_obj->get_rootNode();
        }

};

// class AI{
//     private:
//         AI(){}
//         GameObject* m_obj;
//     public:
//         static AI* get_instance(){
//             static AI ai;
//             return &ai;
//         }
//         GameObject* get_GameObject(){
//             return m_obj;
//         }

//         void set_GameObject(GameObject* obj){
//             m_obj = obj;
//         }

//         SceneNode* get_root_node(){
//             return m_obj->get_rootNode();
//         }

// };