// Termm-Fall 2022
#define POS_SCALE 100
#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "Commands.hpp"
#include "SceneNode.hpp"
#include "Picker.hpp"
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


class A3 : public CS488Window, public Visitor {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	void resetAll();
	void resetJoints();
	void resetPosition();
	void resetRotation();
	void resetCmds(){
		// clear undo/redo
		m_command_idx = -1;
		commands.clear();
		// clear log
		m_log = "";
	}
	
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
	void processLuaSceneFile(const std::string & filename);
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

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;
	JointNode* m_headNode = nullptr;

	// option model
	int m_option_model = ModelPosition;

	// sphere removal
	bool m_circle, m_z_buffer, m_backface, m_frontface;

	// mouse click
	bool m_left_click = false;
	bool m_middle_click = false;
	bool m_right_click = false;

	float m_prev_x;
	float m_prev_y;

	// undo/redo 
	glm::mat4 m_selected_prev_m;
	bool m_first_press = false;
	std::vector<Command> commands;
	int m_command_idx = -1;
	std::string m_log;

	void redo();
	void undo();

	// recursive render
	virtual void visit(SceneNode* node){
		renderSceneGraph(*node);
	}

	// picking
	void get_cursor_pos(double &xpos, double &ypos){
		glfwGetCursorPos( m_window, &xpos, &ypos );
		xpos *= double(m_framebufferWidth) / double(m_windowWidth);
		ypos = m_windowHeight - ypos;
		ypos *= double(m_framebufferHeight) / double(m_windowHeight);
	}

	Picker* m_picker;
	std::unordered_map<JointNode*, std::pair<float, float>> selected_joints;
	

	void rotate_all_selected_joints(double xAngle, double yAngle);
	void push_command();
	void save_selected_joints(){
		for (auto& node : selected_joints){
			node.second.first = node.first->m_angleX;
			node.second.second = node.first->m_angleY;
			//std::cout << *node.first << " x angle is " << node.second.first << " y angle is " << node.second.second << std::endl;
		}
		
	}

	// virtual trackball
	glm::vec3 m_track_last_pos;
	glm::vec3 m_track_curr_pos;

	void trackball(float fOldX,float fNewX, float fOldY, float fNewY); 
	
};
