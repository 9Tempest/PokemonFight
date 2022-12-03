// Termm-Fall 2022

#include "GameWindow.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>
#include "include.hpp"
#include "PlayerAI.hpp"
#include "GameObject.hpp"
#include "random.hpp"


using namespace glm;
using namespace std;

static bool show_gui = false;

mat4 GameWindow::m_view;
mat4 GameWindow::m_view_original;
float GameWindow::m_shake_remaining_time = 0.0f;
float GameWindow::m_shake_time = 0.0f;
float GameWindow::m_shake_remaining_force = 0.0f;
float GameWindow::m_shake_force = 0.0f;

unordered_map<std::string, SceneNode*> Scene::m_elements2d;

void GameWindow::play_music(const std::string& music_name, bool is_loop){
	#ifdef UNMUTED
	if (m_sound != nullptr){
		m_sound->stop();
	}
	m_sound = SoundEngine::play2D(music_name, is_loop, 0.1f);
	assert(m_sound != nullptr);
	std::cout << "playing " << music_name << std::endl;
	m_sound->setVolume(0.1f);
	#else

	#endif
}

void GameWindow::set_ui_hp(GameObject* obj){
	// use name to lookup scene node
	std::string node_name = "hp_bar_";
	node_name += obj->get_name();

	// original scale factor is 5.0
	const float hp_bar_scale = 5.0f;
	float hp_max = obj->get_hp_max();
	float hp = obj->get_hp();
	if (hp < 0) hp = 0;

	// calculate current scale and move bar
	float curr_len = hp_bar_scale * (hp / hp_max);
	float offset = (hp_bar_scale - curr_len);
	assert(Scene::m_elements2d.find(node_name) != Scene::m_elements2d.end());
	SceneNode* hp_bar = Scene::m_elements2d[node_name];
	hp_bar->trans[0][0] = curr_len;
	hp_bar->trans[3][0] = hp_bar->originalM[3][0] - offset;
	//hp_bar->translate(vec3(-offset, 0,0));
}

static inline bool is_snorlax_idle(){
	return AI::get_instance()->get_GameObject()->get_status() == Status::Idle;
}

ISound* GameWindow::m_sound = nullptr;


static inline void enable_texture(GLuint texturePtr, uint id = 0){
	glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, texturePtr);

}

//----------------------------------------------------------------------------------------
// Constructor
GameWindow::GameWindow(const vector<string> & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_z_buffer(true),
	  m_backface(true),
	  m_frontface(false),
	  m_vao_skybox(0)
{

}

void GameWindow::resetAll(){
	//reset position, orientation and joint angles 
	HumanPlayer::get_instance()->get_root_node()->reset();
}

void GameWindow::resetJoints(){
	//reset position, orientation and joint angles 
	HumanPlayer::get_instance()->get_root_node()->init_joint_angle();
}

void GameWindow::resetPosition(){
	//reset position
	HumanPlayer::get_instance()->get_root_node()->reset_position();
}

void GameWindow::resetRotation(){
	//reset joint angles 
	HumanPlayer::get_instance()->get_root_node()->reset_rotation();
}
//----------------------------------------------------------------------------------------
// Destructor
GameWindow::~GameWindow()
{
	delete m_scene;
}

static void set_anim_pika(const string& name){
	Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name(name);
	assert(ani_ptr);
	HumanPlayer::get_instance()->get_GameObject()->do_animation( *ani_ptr);
}

static void set_anim_snorlax(const string& name){
	Animation* ani_ptr = AnimationLoader::get_instance()->get_animation_by_name(name);
	assert(ani_ptr);
	AI::get_instance()->get_GameObject()->do_animation( *ani_ptr);
}

void GameWindow::init_skybox_vao(){
	glGenVertexArrays(1, &m_vao_skybox);
    glGenBuffers(1, &m_vbo_skybox);
    glBindVertexArray(m_vao_skybox);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skybox);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void GameWindow::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	init_skybox_vao();
	// set up node for ai and HumanPlayer
	setup_player_AI();

	// setup animation loader
	AnimationLoader* l = AnimationLoader::get_instance();
	

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("buckyball.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj"),
			getAssetFilePath("mcube.obj"),
			getAssetFilePath("cone.obj"),
			getAssetFilePath("curve.obj"),
			getAssetFilePath("surtorus.obj"),
			getAssetFilePath("cube_text.obj"),
			getAssetFilePath("plane_text.obj"),
			getAssetFilePath("plane2d.obj"),
			getAssetFilePath("sphere_text.obj")
			
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	m_curr_ts = get_curr_time();
	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.

	// init skybox
	m_skybox->initialize();

	// init texture assets
	TextureAssets::initialize();

	// init shadow mapping
	m_shadowmap.init(m_light);
	

	// init start snorlax
	AI::get_instance()->get_GameObject()->stun();

	SoundEngine::init();

	play_music(SOUND_BACKGROUND_MUSIC, true);

	// set grass
	setup_grass_shader();
	setup_grass_vao();

	// 2d
	m_2d_objs->accept(*m_scene);
	projection2d = glm::ortho(0.0f, float(m_windowWidth),float(m_windowHeight), 0.0f, -1.0f, 1.0f);  
}

void GameWindow::setup_player_AI(){
	auto snorlax_rootNode = processLuaSceneFile(m_luaSceneFile[1]);
	AI::get_instance()->set_GameObject(new Snorlax(snorlax_rootNode));
	// load HumanPlayer : Pikachu
	auto pikachu_rootNode = processLuaSceneFile(m_luaSceneFile[0]);
	HumanPlayer::get_instance()->set_GameObject(new Pikachu(pikachu_rootNode));
	// TODO: load skybox
	auto scene_node = processLuaSceneFileAndLoadSkyBox(m_luaSceneFile[2]);
	m_scene = new Scene(scene_node);

	// load 2d objects
	m_2d_objs = processLuaSceneFile(m_luaSceneFile[3]);

}

std::shared_ptr<SceneNode> GameWindow::processLuaSceneFileAndLoadSkyBox(const std::string & filename){
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// This version of the code treats the main program argument
	// as a straightforward pathname.
	auto rootNode = std::shared_ptr<SceneNode>(import_lua(filename, m_skybox));
	if (!rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
	if (!m_skybox){
		std::cerr << "Skybox loaded failed! " << std::endl;
	}
	// build parent hierarchy
	rootNode->add_parent();
	// init joint initial angle
	rootNode->init_joint_angle(true);
	rootNode->parent = nullptr;
	return rootNode;
}

//----------------------------------------------------------------------------------------
std::shared_ptr<SceneNode> GameWindow::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// This version of the code treats the main program argument
	// as a straightforward pathname.
	auto rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
	// build parent hierarchy
	rootNode->add_parent();
	// init joint initial angle
	rootNode->init_joint_angle(true);
	rootNode->parent = nullptr;
	return rootNode;
	
}

void GameWindow::render_scene(ShaderProgram& prog){
	set_shader(prog);

	HumanPlayer::get_instance()->get_root_node()->accept(*this);
	AI::get_instance()->get_root_node()->accept(*this);
	renderParticles(*ParticleAssets::fetch_system("dirt"));
	renderParticles(*ParticleAssets::fetch_system("lightning"));
	renderParticles(*ParticleAssets::fetch_system("electornics"));
	renderParticles(*ParticleAssets::fetch_system("meteorite"));
	m_scene->render(*this);

	m_curr_shader_ptr = nullptr;
}

//----------------------------------------------------------------------------------------
void GameWindow::createShaderProgram()
{
	// phong shader	
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("Phong.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("Phong.fs").c_str() );
	m_shader.link();

	// skybox shader
	m_shader_skybox.generateProgramObject();
	m_shader_skybox.attachVertexShader( getAssetFilePath("skybox.vs").c_str() );
	m_shader_skybox.attachFragmentShader( getAssetFilePath("skybox.fs").c_str() );
	m_shader_skybox.link();

	// shadow map depth shader
	m_shader_shadow_depth.generateProgramObject();
	m_shader_shadow_depth.attachVertexShader( getAssetFilePath("shadow_depth.vs").c_str() );
	m_shader_shadow_depth.attachFragmentShader( getAssetFilePath("shadow_depth.fs").c_str() );
	m_shader_shadow_depth.link();

	// grass shader
	m_shader_grass.generateProgramObject();
	m_shader_grass.attachVertexShader( getAssetFilePath("grass.vs").c_str() );
	m_shader_grass.attachGeometryShader( getAssetFilePath("grass.geo").c_str() );
	m_shader_grass.attachFragmentShader( getAssetFilePath("grass.fs").c_str() );
	m_shader_grass.link();

}

//----------------------------------------------------------------------------------------
void GameWindow::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		m_uvAttribLocation = m_shader.getAttribLocation("aTexCoords");
		glEnableVertexAttribArray(m_uvAttribLocation);

		m_positionAttribLocationShadow = m_shader_shadow_depth.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocationShadow);
		CHECK_GL_ERRORS;
	}


	// Restore defaults
	glBindVertexArray(0);
}

void GameWindow::render2d(){
	set_shader(m_shader);
	m_shader.enable();
	GLint location = m_shader.getUniformLocation("is2d");
	glUniform1i(location, 1);
	m_shader.disable();
	CHECK_GL_ERRORS;


	m_2d_objs->accept(*this);

	
	m_shader.enable();
	glUniform1i(location, 0);
	m_shader.disable();
	CHECK_GL_ERRORS;
	
}


//----------------------------------------------------------------------------------------
void GameWindow::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex UV data
	{
		glGenBuffers(1, &m_vbo_vertexUVs);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUVs);
		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexUVBytes(),
			meshConsolidator.getVertexUVDataPtr(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

}

//----------------------------------------------------------------------------------------
void GameWindow::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribPointer(m_positionAttribLocationShadow, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUVs);
	glVertexAttribPointer(m_uvAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void GameWindow::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void GameWindow::initViewMatrix() {
	m_view = glm::lookAt(m_camera_pos, vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void GameWindow::initLightSources() {
	// World-space position
	m_light.position = vec3(3.0f, 30.0f, 10.0f);
	m_light.rgbIntensity = vec3(1.0f,1.0f, 1.0f); // light
}


void GameWindow::uploadShadowMapUniforms() {
	m_shader_shadow_depth.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader_shadow_depth.getUniformLocation("lightSpaceMatrix");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_shadowmap.lightSpaceMatrix));
		CHECK_GL_ERRORS;
	}

	m_shader_shadow_depth.disable();
}


void GameWindow::setup_grass_shader(){
	m_shader_grass.enable();

	{
		// set location for grass texture
		GLint location = m_shader_grass.getUniformLocation("u_textgrass");
		glUniform1i(location, 0);
		CHECK_GL_ERRORS;
	}
	{
		// set location for wind map
		GLint location = m_shader_grass.getUniformLocation("u_wind");
		glUniform1i(location, 1);
		CHECK_GL_ERRORS;
	}

	{
		// perspective
		GLint location = m_shader_grass.getUniformLocation("u_projection");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;
	}

	m_shader_grass.disable();
}

// randomly generate a cluster of grass
static void generate_grasses(float x_pos, float z_pos, vector<vec3>& grass_positions){
	for(float x = -10.0f; x < 10.0f; x+=0.5f)
        for(float z = -5.0f; z < 5.0f; z+=0.5f)
        {
            int randNumberX = rand() % 10 + 1;
            int randNumberZ = rand() % 10 + 1;
            grass_positions.push_back(glm::vec3(x_pos+x+(float)randNumberX, GROUND, z_pos + z+(float)randNumberZ));
        }
}


void GameWindow::setup_grass_vao(){
	
	// generate 10-15 grass cluster
	int random_num = Random::Int(10, 15);
	for (int i = 0; i < random_num; i++){
		float x_pos;
		float z_pos;
		// carefully generate zpos to circumvent mud road
		if (i < random_num / 5){
			x_pos = Random::Float() * 20;
			z_pos = -Random::FloatPositive() * 15;
		}	else {
			x_pos = Random::Float() * 100;
			z_pos = - 40 - Random::FloatPositive() * 20;
		}
		generate_grasses(x_pos, z_pos, m_grass_positions);
	}
	glGenVertexArrays(1, &m_grass_vao);
    glGenBuffers(1, &m_grass_vbo);
    glBindVertexArray(m_grass_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_grass_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_grass_positions.size() * sizeof(glm::vec3), m_grass_positions.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void GameWindow::renderGrassMain(){
	m_shader_grass.enable();

	 // bind textures
	glActiveTexture(GL_TEXTURE0);
	Texture* text = TextureAssets::get_asset("grass_texture.png");
	assert(text != nullptr);
	enable_texture(text->texturePtr, 0);
	CHECK_GL_ERRORS;
	
	text = TextureAssets::get_asset("flowmap.png");
	assert(text != nullptr);
	enable_texture(text->texturePtr, 1);
	CHECK_GL_ERRORS;

	// update view
	{
		// set location for view
		GLint location = m_shader_grass.getUniformLocation("u_view");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_view));
		CHECK_GL_ERRORS;
	}
	{
		// set location for camera pos
		GLint location = m_shader_grass.getUniformLocation("u_cameraPosition");
		glUniform3fv(location, 1, value_ptr(m_camera_pos));
		CHECK_GL_ERRORS;
	}
	{
		// set time
		GLint location = m_shader_grass.getUniformLocation("u_time");
		glUniform1f(location, glfwGetTime());
		CHECK_GL_ERRORS;
	}

	glBindVertexArray(m_grass_vao);
    glDrawArrays(GL_POINTS, 0, m_grass_positions.size());

	m_shader_grass.disable();
}



//----------------------------------------------------------------------------------------
void GameWindow::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.3f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}

		// set shadowMap location
		{
			location = m_shader.getUniformLocation("shadow");
			glUniform1i(location, 1);
			
			CHECK_GL_ERRORS;
		}

		// set texture location
		{
			location = m_shader.getUniformLocation("texture1");
			glUniform1i(location, 0);
			CHECK_GL_ERRORS;
		}

		{
			//-- Set Perpsective matrix uniform for the scene:
			location = m_shader.getUniformLocation("lightSpaceMatrix");
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_shadowmap.lightSpaceMatrix));
			CHECK_GL_ERRORS;
		}

		{
			// --Set Toon shading uniform
			location = m_shader.getUniformLocation("enableToon");
			glUniform1i(location, (int)m_enableToonShading);
			CHECK_GL_ERRORS;
		}

		
		

	}
	m_shader.disable();
}



void GameWindow::processCameraShake(){
	if (m_shake_remaining_time <= 0.0f) return;
	float time_diff = get_time_diff(m_curr_ts, get_curr_time());
	m_shake_remaining_time -= time_diff;
	
	
	// if end, recovery m_view
	if (m_shake_remaining_time <= 0.0f){
		m_shake_remaining_time = 0.0f;
		m_shake_time = 0.0f;
		m_view = m_view_original;
		return;
	}
	m_shake_remaining_force = m_shake_force * (m_shake_remaining_time / m_shake_time);
	vec3 transl = m_shake_remaining_force * vec3(Random::Float(), Random::Float(), Random::Float());
	m_view =  glm::translate(transl) * m_view_original;

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void GameWindow::appLogic()
{
	// if snorlax is idle, let him attack
	if (is_snorlax_idle()){
		// change it to general attack method
		AI::get_instance()->get_GameObject()->attack("BodySlam", HumanPlayer::get_instance()->get_GameObject());
	}
	
	HumanPlayer::get_instance()->get_GameObject()->update();
	AI::get_instance()->get_GameObject()->update();
	ParticleAssets::fetch_system("dirt")->update();
	ParticleAssets::fetch_system("meteorite")->update(true, meteorite_destroy, true);
	ParticleAssets::fetch_system("lightning")->update(false);
	ParticleAssets::fetch_system("electornics")->update(false);
	processCameraShake();
	uploadCommonSceneUniforms();
	uploadShadowMapUniforms();
	m_curr_ts = get_curr_time();
	//cout << "frame counter is " << time(NULL) << endl;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void GameWindow::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);


	// menu panel
	ImGui::Begin("Menu", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		
		ImGui::Text( "Snorlax hp: %d", AI::get_instance()->get_GameObject()->get_hp());
		ImGui::Text( "Pikachu hp: %d", HumanPlayer::get_instance()->get_GameObject()->get_hp());

	ImGui::End();

	
}



void GameWindow::shadow_processing(){
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowmap.depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
			render_scene(m_shader_shadow_depth);
            
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameWindow::scene_processing(){
	glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	enable_texture(m_shadowmap.depthMap, 1);
	render_scene(m_shader);
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniformsScene(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		bool is_shadow=false
) {

	shader.enable();
	if (!is_shadow){
		{
			//-- Set ModelView matrix:
			GLint location = shader.getUniformLocation("Model");
			mat4 modelView = viewMatrix *  node.trans;
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(node.trans));
			CHECK_GL_ERRORS;
			
			// - Set View matrix:
			location = shader.getUniformLocation("View");
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(viewMatrix));
			CHECK_GL_ERRORS;

			//-- Set NormMatrix:
			location = shader.getUniformLocation("NormalMatrix");
			mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;


			//-- Set Material values:
			location = shader.getUniformLocation("material.kd");
			vec3 kd;
			kd = node.material.kd;
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;

			// enable texture mapping or not
			location = shader.getUniformLocation("enabletexture");
			int enable_texture_uni = 0;
			if (node.m_texture != nullptr){
				enable_texture_uni = 1;
				enable_texture(node.m_texture->texturePtr);
			}
			glUniform1i(location, enable_texture_uni);
			CHECK_GL_ERRORS;
		}}	else {	// shadow mat
			//-- Set ModelView matrix:
			GLint location = shader.getUniformLocation("model");
			mat4 model = node.trans;
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
			CHECK_GL_ERRORS;
		}	// if
	
	shader.disable();

}


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void GameWindow::draw() {


	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable( GL_DEPTH_TEST );
	shadow_processing();
	scene_processing();
	renderGrassMain();
	render2d();
	renderSkyBox();
	glDisable(GL_CULL_FACE);
	glDisable( GL_DEPTH_TEST );

	
}

static inline mat4 calc_rotation_mat(const vec3& xyz){
	return glm::rotate(degreesToRadians(xyz.x), vec3(1,0,0)) * glm::rotate(degreesToRadians(xyz.y), vec3(0,1,0)) * glm::rotate(degreesToRadians(xyz.z), vec3(0,0,1));
}

void GameWindow::renderSkyBox(){
	glDepthMask(GL_FALSE);
	//glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	assert(m_skybox != nullptr);
	// bind vao
	glBindVertexArray(m_vao_skybox);
	// bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox->texturePtr);
	CHECK_GL_ERRORS;
	m_shader_skybox.enable();
	{
		//-- Set ModelView matrix:
		GLint location = m_shader_skybox.getUniformLocation("View");
		mat4 View = mat4(mat3(m_view));
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(View));
		CHECK_GL_ERRORS;

		//-- Set Perspective:
		location = m_shader_skybox.getUniformLocation("Perspective");
		mat4 persp = m_perpsective;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(persp));
		CHECK_GL_ERRORS;

		//-- Set sampler:
		location = m_shader_skybox.getUniformLocation("skybox");
		glUniform1i(location, 0);
		CHECK_GL_ERRORS;

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	m_shader_skybox.disable();
	//glDepthFunc(GL_LESS); // set depth function back to default
	glDepthMask(GL_TRUE);
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void GameWindow::renderParticles(const ParticleSystem& ps){
	// Bind the VAO once here, and reuse for all GeometryNode rendering below.

	//assert(ParticleAssets::assets.find(ps.get_name()) != ParticleAssets::assets.end());
	GeometryNode* geo = ParticleAssets::fetch_mesh(ps.get_name());
	assert(geo != nullptr);
	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	for (auto& p : ps.m_pool){
		if (p.m_active){
			geo->trans = glm::translate(p.m_position) * calc_rotation_mat(p.m_rot) * scale(vec3(p.m_size, p.m_size, p.m_size)) * geo->originalM;
			updateShaderUniformsScene(*m_curr_shader_ptr, *geo, m_view, m_curr_shader_ptr == &m_shader_shadow_depth);

			// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
			BatchInfo batchInfo = m_batchInfoMap[geo->meshId];

			//-- Now render the mesh:
			m_curr_shader_ptr->enable();
			glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
			m_curr_shader_ptr->disable();
		}	// if
	}	// for

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
	//cout << "printing geo " << geo->m_name << endl;
}

//----------------------------------------------------------------------------------------
void GameWindow::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.

	for (const SceneNode * node : root.children) {

		if (node->m_nodeType != NodeType::GeometryNode)
			continue;

		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);


		updateShaderUniformsScene(*m_curr_shader_ptr, *geometryNode, m_view, m_curr_shader_ptr == &m_shader_shadow_depth);


		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_curr_shader_ptr->enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_curr_shader_ptr->disable();
	}

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}



//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void GameWindow::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool GameWindow::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}


//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool GameWindow::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool GameWindow::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}


void static inline emit_dirt_test(){
	GameObject* snorlaxobj = AI::get_instance()->get_GameObject();
	dirt_flying_effect(((Snorlax*)snorlaxobj)->get_radius(), snorlaxobj->get_pos());
}


const float STEP_SIZE = 5.0f;

static inline bool is_pikachu_idle(){
	float remaining_time = HumanPlayer::get_instance()->get_GameObject()->get_remaining_time();
	cout << " remaining time is " << remaining_time << endl;
	return  remaining_time < 0.1f;
}

static inline bool is_pikachu_moving(){
	return HumanPlayer::get_instance()->get_GameObject()->get_status() == Status::Moving;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool GameWindow::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if (key == GLFW_KEY_Q){
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_S){
			if (is_pikachu_idle()){
				set_anim_pika("pikachu_move");
				HumanPlayer::get_instance()->get_GameObject()->set_orientation(Orientation::Down);
				HumanPlayer::get_instance()->get_GameObject()->move(0,STEP_SIZE);
			}
			
			eventHandled = true;
		}
		if (key == GLFW_KEY_A){
			if (is_pikachu_idle()){
				set_anim_pika("pikachu_move");
				HumanPlayer::get_instance()->get_GameObject()->set_orientation(Orientation::Left);
				HumanPlayer::get_instance()->get_GameObject()->move(-STEP_SIZE,0);
			}
			eventHandled = true;
		}
		if (key == GLFW_KEY_D){
			if (is_pikachu_idle()){
				set_anim_pika("pikachu_move");
				HumanPlayer::get_instance()->get_GameObject()->set_orientation(Orientation::Right);
				HumanPlayer::get_instance()->get_GameObject()->move(STEP_SIZE,0);
			}
			eventHandled = true;
		}
		if (key == GLFW_KEY_W){
			if (is_pikachu_idle()){
				set_anim_pika("pikachu_move");
				HumanPlayer::get_instance()->get_GameObject()->set_orientation(Orientation::Up);
				HumanPlayer::get_instance()->get_GameObject()->move(0,-STEP_SIZE);
			}
			eventHandled = true;
		}
		if (key == GLFW_KEY_J){
			if (is_pikachu_idle()){
				HumanPlayer::get_instance()->get_GameObject()->attack("discharge_right", AI::get_instance()->get_GameObject());
			}
			eventHandled = true;
		}
		if (key == GLFW_KEY_K){
			if (is_pikachu_idle()){
				HumanPlayer::get_instance()->get_GameObject()->attack("discharge_left", AI::get_instance()->get_GameObject());
			}
			eventHandled = true;
		}
		if (key == GLFW_KEY_T){
			m_enableToonShading = !m_enableToonShading;
			eventHandled = true;
		}
		



	}
	// Fill in with event handling code...

	return eventHandled;
}
