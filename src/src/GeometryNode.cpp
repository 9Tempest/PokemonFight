// Termm-Fall 2022

#include "GeometryNode.hpp"
#include "JointNode.hpp"
//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

GeometryNode::GeometryNode(const GeometryNode& other):SceneNode(other), material(other.material), meshId(other.meshId){
	for(SceneNode * child : other.children) {
		if (child->m_nodeType == NodeType::GeometryNode){
			this->children.push_back(new GeometryNode(*(GeometryNode*)child));
		}	else if (child->m_nodeType == NodeType::JointNode){
			this->children.push_back(new JointNode(*(JointNode*)child));
		}	else {
			this->children.push_front(new SceneNode(*child));
		}
	}
}


JointNode* GeometryNode::get_upper_joint(){
	SceneNode* curr = parent;
	while( curr != nullptr && curr->m_nodeType != NodeType::JointNode){
		curr = curr->parent;
		
	}
	
	return (JointNode*)curr;
}
