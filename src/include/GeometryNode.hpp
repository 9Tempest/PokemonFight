// Termm-Fall 2022

#pragma once

#include "SceneNode.hpp"

class JointNode;

class GeometryNode : public SceneNode {

public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	GeometryNode(const GeometryNode& other);

	JointNode* get_upper_joint();
	

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;

};
