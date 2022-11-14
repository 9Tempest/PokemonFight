// Termm-Fall 2022

#pragma once

#include "Material.hpp"
#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>
class SceneNode;

class Visitor{
    public:
    virtual void visit(SceneNode* node) = 0;
};

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class SceneNode {

public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;

    virtual void reset();
    virtual void reset_position();
    virtual void reset_rotation();
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);

    // add this to each children's parent; should only be called by Root
    void add_parent();
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void rotatelocal(char axis, float angle, const glm::mat4& recoveryM, bool initial = false);
    void rotatelocalxy(float angleX, float angleY, const glm::mat4& recoveryM);

    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);
    void applyTranslate(const glm::vec3& amount);


    void transform(const glm::mat4& mat);


    void print_all(int intend = 0);
	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    virtual void init_joint_angle(bool is_init=false){
        for (auto child : children){
            child->init_joint_angle(is_init);
        }
    }
    
    void applyRotMat(const glm::mat4& rot, const glm::mat4& recoveryM);


	bool isSelected;
    
    // Transformations
    glm::mat4 trans = glm::mat4();    // general transformation
    glm::mat4 invtrans = glm::mat4();
    glm::mat4 originalM = glm::mat4();
    glm::mat4 translateM = glm::mat4();
    glm::mat4 rotationM = glm::mat4();
    
    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;

    // register node
    virtual void accept(Visitor& p){
        p.visit(this);
        for (auto child : children){
            child->accept(p);
        }
    }

    // parent ptr for locate joints
    SceneNode* parent = nullptr;


private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
