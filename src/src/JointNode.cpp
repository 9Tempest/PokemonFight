// Termm-Fall 2022

#include "JointNode.hpp"
#include "GeometryNode.hpp"
//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

JointNode::JointNode(const JointNode& other):
	SceneNode(other), m_joint_x(other.m_joint_x), m_joint_y(other.m_joint_y), m_angleX(other.m_angleX), m_angleY(other.m_angleY)
	{
	}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

void JointNode::rotate_joint(double xAngle, double yAngle){
	double angle_after;
	double x_tmp, x_offset, y_tmp, y_offset;
	angle_after = m_angleX + xAngle;
	x_tmp = m_angleX;
	if (xAngle > 0){
		m_angleX = (angle_after > m_joint_x.max) ? m_joint_x.max : angle_after;
	}	else {
		m_angleX = (angle_after < m_joint_x.min) ? m_joint_x.min : angle_after;
	}
	x_offset = m_angleX-x_tmp;

	angle_after = m_angleY + yAngle;
	y_tmp = m_angleY;
	if (yAngle > 0){
		m_angleY = (angle_after > m_joint_y.max) ? m_joint_y.max : angle_after;
	}	else {
		m_angleY = (angle_after < m_joint_y.min) ? m_joint_y.min : angle_after;
	}
	y_offset = m_angleY-y_tmp;
	for (auto child : children){
		child->rotatelocalxy(x_offset, y_offset, trans);
	}
	
}