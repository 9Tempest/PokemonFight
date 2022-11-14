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

JointNode* GeometryNode::get_upper_joint(){
	SceneNode* curr = parent;
	while( curr != nullptr && curr->m_nodeType != NodeType::JointNode){
		curr = curr->parent;
		
	}
	
	return (JointNode*)curr;
}
