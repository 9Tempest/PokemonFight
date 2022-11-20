// Termm-Fall 2022
#define POS_SCALE 100
#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "SceneNode.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include "JointNode.hpp"
#include <iostream>
enum OptionModel {
	ModelPosition,
	ModelJoints,

	ModelNone,
};

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


class GameWindow : public CS488Window, public Visitor {
public:
	GameWindow(const std::vector<std::string> & luaSceneFiles);
	virtual ~GameWindow();

protected:
	virtual void init() override;
	void resetAll();
	void resetJoints();
	void resetPosition();
	void resetRotation();
	
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	std::shared_ptr<SceneNode> processLuaSceneFile(const std::string & filename);
	void setup_player_AI();
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode &node);
	void renderArcCircle();

	void processPicking();

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::vector<std::string> m_luaSceneFile;

	JointNode* m_headNode = nullptr;

	// option model
	int m_option_model = ModelPosition;

	// sphere removal
	bool m_z_buffer, m_backface, m_frontface;

	// mouse click
	bool m_left_click = false;
	bool m_middle_click = false;
	bool m_right_click = false;

	float m_prev_x;
	float m_prev_y;

	// undo/redo 
	glm::mat4 m_selected_prev_m;
	bool m_first_press = false;
	std::string m_log;


	// recursive render
	virtual void visit(SceneNode* node) override{
		renderSceneGraph(*node);
	}

	std::unordered_map<JointNode*, std::pair<float, float>> selected_joints;
	

	void rotate_all_selected_joints(double xAngle, double yAngle);
	void save_selected_joints(){
		for (auto& node : selected_joints){
			node.second.first = node.first->m_angleX;
			node.second.second = node.first->m_angleY;
			//std::cout << *node.first << " x angle is " << node.second.first << " y angle is " << node.second.second << std::endl;
		}
		
	}

	
};