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
static bool show_gui = true;

mat4 GameWindow::m_view;
mat4 GameWindow::m_view_original;
float GameWindow::m_shake_remaining_time = 0.0f;
float GameWindow::m_shake_time = 0.0f;
float GameWindow::m_shake_remaining_force = 0.0f;
float GameWindow::m_shake_force = 0.0f;
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
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj"),
			getAssetFilePath("mcube.obj"),
			getAssetFilePath("cone.obj"),
			getAssetFilePath("curve.obj"),
			getAssetFilePath("surtorus.obj"),
			getAssetFilePath("cube_text.obj"),
			getAssetFilePath("plane_text.obj"),
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

//----------------------------------------------------------------------------------------
void GameWindow::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("Phong.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("Phong.fs").c_str() );
	m_shader.link();

	m_shader_skybox.generateProgramObject();
	m_shader_skybox.attachVertexShader( getAssetFilePath("skybox.vs").c_str() );
	m_shader_skybox.attachFragmentShader( getAssetFilePath("skybox.fs").c_str() );
	m_shader_skybox.link();
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

		CHECK_GL_ERRORS;
	}


	// Restore defaults
	glBindVertexArray(0);
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
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 20.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void GameWindow::initLightSources() {
	// World-space position
	m_light.position = vec3(0.0f, 10.0f, 0.0f);
	m_light.rgbIntensity = vec3(1.0f,1.0f, 1.0f); // light
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

		// set texture location
		{
			location = m_shader.getUniformLocation("texture1");
			glUniform1i(location, 0);
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
		m_view = m_view_original;
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
	// Place per frame, application logic here ...
	if (m_backface && m_frontface){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	}	else if (m_backface){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}	else if (m_frontface){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}	else {
		glDisable(GL_CULL_FACE);
	}
	HumanPlayer::get_instance()->get_GameObject()->update();
	AI::get_instance()->get_GameObject()->update();
	ParticleAssets::fetch_system("dirt")->update();
	ParticleAssets::fetch_system("lightning")->update(false);
	ParticleAssets::fetch_system("electornics")->update(false);
	processCameraShake();
	uploadCommonSceneUniforms();
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

		// Application panel
		if (ImGui::BeginMenu("Application")){
			// Create Button, and check if it was clicked:
			if( ImGui::Button( "Reset Position (I)" ) ) {
				resetPosition();
			}
			if( ImGui::Button( "Reset Orientation (O)" ) ) {
				resetRotation();
			}
			if( ImGui::Button( "Reset Joints (J)" ) ) {
				resetJoints();
			}
			if( ImGui::Button( "Reset All (A)" ) ) {
				resetAll();
			}
			if( ImGui::Button( "Quit (Q)" ) ) {
				glfwSetWindowShouldClose(m_window, GL_TRUE);
			}
			ImGui::EndMenu();
		}	// if

		// Option panel
		if (ImGui::BeginMenu("Option")){
			// Create Button, and check if it was clicked:
			if (ImGui::Checkbox("Z-buffer (Z)", &m_z_buffer)){}
			if (ImGui::Checkbox("Backface culling (B)", &m_backface)){}
			if (ImGui::Checkbox("Frontface culling (F)", &m_frontface)){}
			ImGui::EndMenu();
		}	// if

		if( ImGui::RadioButton( "Position/Orientation (P)", &m_option_model, ModelPosition ) ) {}
		if( ImGui::RadioButton( "Joints (J)", &m_option_model, ModelJoints ) ) {}
		// Add more gui elements here here ...


		
		ImGui::Text( "Log: %s", m_log.c_str());
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	
}

static inline void enable_texture(Texture* text){
	assert(text != nullptr);
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text->texturePtr);

}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		const glm::mat4 & scale_m = mat4()
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * scale_m * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
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
			enable_texture(node.m_texture);
		}
		glUniform1i(location, enable_texture_uni);
		CHECK_GL_ERRORS;
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void GameWindow::draw() {


	glEnable( GL_DEPTH_TEST );
	HumanPlayer::get_instance()->get_root_node()->accept(*this, scale(vec3(1.0, 1.0, 1.0)));
	AI::get_instance()->get_root_node()->accept(*this, scale(vec3(1.0f, 1.0f, 1.0f)));
	renderParticles(*ParticleAssets::fetch_system("dirt"));
	renderParticles(*ParticleAssets::fetch_system("lightning"));
	renderParticles(*ParticleAssets::fetch_system("electornics"));
	m_scene->render(*this);
	renderSkyBox();
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
			updateShaderUniforms(m_shader, *geo, m_view);

			// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
			BatchInfo batchInfo = m_batchInfoMap[geo->meshId];

			//-- Now render the mesh:
			m_shader.enable();
			glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
			m_shader.disable();
		}	// if
	}	// for

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
	//cout << "printing geo " << geo->m_name << endl;
}

//----------------------------------------------------------------------------------------
void GameWindow::renderSceneGraph(const SceneNode & root, const mat4& scale_m) {

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

		updateShaderUniforms(m_shader, *geometryNode, m_view, scale_m);


		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
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


const float STEP_SIZE = 3.0f;

static inline bool is_pikachu_idle(){
	return HumanPlayer::get_instance()->get_GameObject()->get_status() == Status::Idle;
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
		if (key == GLFW_KEY_J){
			set_anim_pika("pikachu_attack");
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
		if (key == GLFW_KEY_O){
			set_anim_snorlax("snorlax_bodyslam_up");
			eventHandled = true;
		}
		if (key == GLFW_KEY_L){
			set_anim_snorlax("snorlax_bodyslam_down");
			eventHandled = true;
		}
		if (key == GLFW_KEY_T){
			if (is_pikachu_idle()){
				HumanPlayer::get_instance()->get_GameObject()->attack("discharge", AI::get_instance()->get_GameObject());
			}
			eventHandled = true;
		}
		if (key == GLFW_KEY_B){
			AI::get_instance()->get_GameObject()->attack("BodySlam", HumanPlayer::get_instance()->get_GameObject());
		}
		



	}
	// Fill in with event handling code...

	return eventHandled;
}
