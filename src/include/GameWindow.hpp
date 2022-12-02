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
#include "particle.hpp"
#include <iostream>
#include "texture.hpp"
#include "Scene.hpp"
#include "sound.hpp"
#include "shadow.hpp"
enum OptionModel {
	ModelPosition,
	ModelJoints,

	ModelNone,
};



const float SHAKE_STRENGTH = 2.0f;

class GameWindow : public CS488Window, public Visitor {
public:
	GameWindow(const std::vector<std::string> & luaSceneFiles);
	virtual ~GameWindow();
	// camera shake
	static void cameraShake(float duration, float force = SHAKE_STRENGTH){
		m_view_original = m_view;
		if (duration > m_shake_time){
			m_shake_time = duration;
		}
		if (duration > m_shake_remaining_time){
			m_shake_remaining_time = duration;
		}
		m_shake_force = force;
		m_shake_remaining_force = force;
	}

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
	//virtual bool mouseMoveEvent(double xPos, double yPos) override;
	//virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	std::shared_ptr<SceneNode> processLuaSceneFile(const std::string & filename);
	std::shared_ptr<SceneNode> processLuaSceneFileAndLoadSkyBox(const std::string & filename);
	void setup_player_AI();
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();
	void init_skybox_vao();
	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void uploadShadowMapUniforms();
	void renderSceneGraph(const SceneNode &node , const glm::mat4& scale_m = glm::mat4());
	void renderParticles(const ParticleSystem& ps);
	void renderSkyBox();

	void processCameraShake();

	glm::mat4 m_perpsective;
	static glm::mat4 m_view;
	static glm::mat4 m_view_original;
	static float m_shake_time;
	static float m_shake_remaining_time;
	static float m_shake_force;
	static float m_shake_remaining_force;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_positionAttribLocationShadow;
	GLuint m_vbo_vertexNormals;
	GLuint m_vbo_vertexUVs;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	GLint m_uvAttribLocation;
	ShaderProgram m_shader;
	ShaderProgram m_shader_skybox;
	ShaderProgram m_shader_shadow_depth;

	ShaderProgram* m_curr_shader_ptr = nullptr;

	void set_shader(ShaderProgram& prog){
		m_curr_shader_ptr = &prog;
	}

	void shadow_processing();
	void scene_processing();
	void render_scene(ShaderProgram& prog);

	// skybox
	GLuint m_vao_skybox;
	GLuint m_vbo_skybox;
	SkyBox* m_skybox = nullptr;

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

	time_stamp m_curr_ts;

	std::string m_log;

	// scene
	Scene* m_scene;

	// recursive render
	virtual void visit(SceneNode* node, const glm::mat4& scale_m = glm::mat4()) override{
		renderSceneGraph(*node, scale_m);
	}
	// Shadow mapping
	ShadowMap m_shadowmap;
	// background music
	static irrklang::ISound* m_sound;
	// tool shading
	bool m_enableToonShading = false;
	public:
	static void play_music(const std::string& music_name, bool is_loop = false);


	
};


const float skyboxVertices[] = {
        // positions          
        -100.0f,  100.0f, -100.0f,
        -100.0f, -100.0f, -100.0f,
         100.0f, -100.0f, -100.0f,
         100.0f, -100.0f, -100.0f,
         100.0f,  100.0f, -100.0f,
        -100.0f,  100.0f, -100.0f,

        -100.0f, -100.0f,  100.0f,
        -100.0f, -100.0f, -100.0f,
        -100.0f,  100.0f, -100.0f,
        -100.0f,  100.0f, -100.0f,
        -100.0f,  100.0f,  100.0f,
        -100.0f, -100.0f,  100.0f,

         100.0f, -100.0f, -100.0f,
         100.0f, -100.0f,  100.0f,
         100.0f,  100.0f,  100.0f,
         100.0f,  100.0f,  100.0f,
         100.0f,  100.0f, -100.0f,
         100.0f, -100.0f, -100.0f,

        -100.0f, -100.0f,  100.0f,
        -100.0f,  100.0f,  100.0f,
         100.0f,  100.0f,  100.0f,
         100.0f,  100.0f,  100.0f,
         100.0f, -100.0f,  100.0f,
        -100.0f, -100.0f,  100.0f,

        -100.0f,  100.0f, -100.0f,
         100.0f,  100.0f, -100.0f,
         100.0f,  100.0f,  100.0f,
         100.0f,  100.0f,  100.0f,
        -100.0f,  100.0f,  100.0f,
        -100.0f,  100.0f, -100.0f,

        -100.0f, -100.0f, -100.0f,
        -100.0f, -100.0f,  100.0f,
         100.0f, -100.0f, -100.0f,
         100.0f, -100.0f, -100.0f,
        -100.0f, -100.0f,  100.0f,
         100.0f, -100.0f,  100.0f
    };
