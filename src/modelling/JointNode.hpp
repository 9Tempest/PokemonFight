// Termm-Fall 2022

#pragma once

#include "SceneNode.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/io.hpp>
class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	JointNode(const JointNode& other);
	virtual ~JointNode();


	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	void rotate_joint(double xAngle, double yAngle);

	virtual void init_joint_angle(bool is_init=false){
		float offset_x, offset_y;
		offset_x = m_angleX - m_joint_x.init;
		offset_y = m_angleY - m_joint_y.init;
		m_angleX = m_joint_x.init;
		m_angleY = m_joint_y.init;
		rotatelocal('z', -offset_x, trans, is_init);
		rotatelocal('y', -offset_y, trans, is_init);
		for (auto child : children){
            child->init_joint_angle(is_init);
			std::cout << " in init joint node " << m_name << " has quat " << glm::quat_cast(jointRotationM) << " has rot " << jointRotationM << std::endl;
        }
	}

	struct JointRange {
		double min, init, max;
	};


	JointRange m_joint_x, m_joint_y;
	double m_angleX = 0.0f;
	double m_angleY = 0.0f;

};
