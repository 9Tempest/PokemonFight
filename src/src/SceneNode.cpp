// Termm-Fall 2022

#include "SceneNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/io.hpp>
#include "GeometryNode.hpp"
#include "JointNode.hpp"
#include "Scene.hpp"
using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
	isSelected(false),
	m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name + std::to_string(nodeInstanceCount)),
	  trans(other.trans),
	  invtrans(other.invtrans),
	  originalM(other.originalM),
	  translateM(other.translateM),
	  rotationM(other.rotationM),
	  m_nodeId(nodeInstanceCount++)
{	
	for(SceneNode * child : other.children) {
		if (child->m_nodeType == NodeType::GeometryNode){
			this->children.push_back(new GeometryNode(*(GeometryNode*)child));
		}	else if (child->m_nodeType == NodeType::JointNode){
				this->children.push_back(new JointNode(*(JointNode*)child));
		}	else {
			this->children.push_back(new SceneNode(*child));
		}
		
	}
}

void SceneNode::reset(){
	// do reset itself first
	trans = originalM;
	translateM = mat4();
	rotationM = mat4();
	for (auto child : children){
		child->reset();
	}
}

void SceneNode::reset_position(){
	// do reset itself first
	trans = inverse(translateM) * trans;
	translateM = mat4();
	for (auto child : children){
		child->reset_position();
	}
}

void SceneNode::reset_rotation(){
	// do reset itself first
	trans = inverse(rotationM) * trans;
	rotationM = mat4();
	for (auto child : children){
		child->reset_rotation();
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	children.remove(child);
}

void SceneNode::rotatelocal(char axis, float angle, const glm::mat4& recoveryM, bool initial){
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = recoveryM * glm::rotate(degreesToRadians(angle), rot_axis) * inverse(recoveryM);
	trans =  rot_matrix  * trans;
	// if it's initial, count this rotation as original not rotationM
	if (initial) {
		originalM = rot_matrix * originalM;
	}
	rotationAndTransl = rot_matrix * rotationAndTransl;
	for (auto child : children){
		child->rotatelocal(axis, angle, recoveryM, initial);
	}
}

void SceneNode::rotatelocalxy(float angleX, float angleY, const glm::mat4& recoveryM){
	mat4 rot_matrix_x = glm::rotate(degreesToRadians(angleX), vec3(0,0,1));
	mat4 rot_matrix_y = glm::rotate(degreesToRadians(angleY), vec3(0,1,0));
	mat4 rot_matrix = recoveryM * rot_matrix_x *rot_matrix_y  * inverse(recoveryM);
	rotationAndTransl = rot_matrix * rotationAndTransl;
	trans = rot_matrix * trans;
	//cout << " 2in init joint node " << m_name << " has quat " << glm::quat_cast(jointRotationM)<<  " has rot " << jointRotationM << std::endl;
	for (auto child : children){
		child->rotatelocalxy(angleX, angleY, recoveryM);
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	originalM = rot_matrix * originalM;
	trans = rot_matrix * trans;
	for (auto child : children){
		child->rotate(axis, angle);
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	mat4 T = glm::scale(amount);
	trans = T * trans;
	originalM = T * originalM;
	scaleM = T * scaleM;
	for (auto child : children){
		child->scale(amount);
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	mat4 T = glm::translate(amount);
	trans = T * trans;
	originalM = T * originalM;
	rotationAndTransl = T * rotationAndTransl;
	for (auto child : children){
		child->translate(amount);
	}
}

void SceneNode::applyTranslate(const glm::vec3& amount){
	mat4 T = glm::translate(amount);
	trans = T * trans;
	translateM = T * translateM;
	rotationAndTransl = T * rotationAndTransl;
	for (auto child : children){
		child->applyTranslate(amount);
	}
}



void SceneNode::applyRotTranslTransform(const glm::mat4& mat){
	rotationAndTransl = mat * rotationAndTransl;
	trans = mat * trans;
	for (auto child : children){
		child->applyRotTranslTransform(mat);
	}
}

//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

void SceneNode::print_all(int intend){
	for (int i = 0; i < intend; i++){
		std::cout << '-';
	}
	std::cout << *this << std::endl;
	mat4 m = this->get_transform();
	//cout << m;
	std::cout << m  << std::endl;
	for (auto child : children){
		child->print_all(intend+1);
		mat4 m = child->get_transform();
		std::cout << m << std::endl;
	}
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;
	os << "]";

	return os;
}

void SceneNode::add_parent(){
	for (auto child:children){
		child->parent = this;
		child->add_parent();
	}
}

void SceneNode::applyRotMat(const glm::mat4& rot, const glm::mat4& recoveryM){
	mat4 R = recoveryM * rot * inverse(recoveryM);
	trans = R * trans;
	rotationM = R * rotationM;
	rotationAndTransl = R * rotationAndTransl;
	for (auto child : children){
		child->applyRotMat(rot, recoveryM);
	}
}


void SceneNode::accept(Visitor& p){

	p.visit(this);
	
	for (auto child : children){
			child->accept(p);
	}
}

