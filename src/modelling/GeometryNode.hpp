// Termm-Fall 2022

#pragma once

#include "SceneNode.hpp"

class JointNode;

const Material HIGHTLIGHT(glm::vec3(140, 255, 50)/255.0f, glm::vec3(1.0f), 1.0f);
class GeometryNode : public SceneNode {
	void unselect(){
		material = original_material;
		isSelected = false;
	}
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	bool select(){
		if (isSelected){
			unselect();
		}	else {
			material = HIGHTLIGHT;
			isSelected = true;
		}
		return isSelected;
	}

	JointNode* get_upper_joint();
	

	Material material;
	Material original_material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;

	glm::vec3 uniq_color;
};
