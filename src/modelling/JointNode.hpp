// Termm-Fall 2022

#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
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
		for (auto child : children){
			child->rotatelocal('z', -offset_x, trans, is_init);
			child->rotatelocal('y', -offset_y, trans, is_init);
            child->init_joint_angle(is_init);
        }
	}

	struct JointRange {
		double min, init, max;
	};


	JointRange m_joint_x, m_joint_y;
	double m_angleX = 0.0f;
	double m_angleY = 0.0f;

};
