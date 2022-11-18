// Termm-Fall 2022

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "trackball.hpp"

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

void A3::trackball(float fOldX,float fNewX, float fOldY, float fNewY){
	float  fRotVecX, fRotVecY, fRotVecZ;
	float fDiameter;
	int iCenterX, iCenterY;
	float fNewModX, fNewModY, fOldModX, fOldModY;
	mat4 rotMat;

	/* vCalcRotVec expects new and old positions in relation to the center of the
		* trackball circle which is centered in the middle of the window and
		* half the smaller of nWinWidth or nWinHeight.
		*/
	fDiameter = (m_windowWidth < m_windowHeight) ? m_windowWidth * 0.5 : m_windowHeight * 0.5;
	iCenterX = m_windowWidth / 2;
	iCenterY = m_windowHeight / 2;
	fOldModX = fOldX - iCenterX;
	fOldModY = fOldY - iCenterY;
	fNewModX = fNewX - iCenterX;
	fNewModY = fNewY - iCenterY;

	vCalcRotVec(fNewModX, fNewModY,
					fOldModX, fOldModY,
					fDiameter,
					&fRotVecX, &fRotVecY, &fRotVecZ);
	/* Negate Y component since Y axis increases downwards
		* in screen space and upwards in OpenGL.
		*/
	vAxisRotMatrix(-fRotVecX, fRotVecY, -fRotVecZ, rotMat);

	m_rootNode->applyRotMat(rotMat, m_rootNode->trans);
}


void A3::undo(){
	if (m_command_idx >= 0){
		commands[m_command_idx].undo();
		m_command_idx--;
		m_log = "Undo succeeded!";
	}	else {
		m_log = "Undo is not allowed!";
	}// if
}	// undo

void A3::redo(){
	int size = commands.size();
	if (m_command_idx < size-1){
		m_command_idx++;
		commands[m_command_idx].redo();
		m_log = "Redo succeeded!";
	}	else {
		m_log = "Redo is not allowed!";
	}// if
}	// undo

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
	  m_circle(false),
	  m_z_buffer(true),
	  m_backface(false),
	  m_frontface(false)
{

}

void A3::resetAll(){
	resetCmds();
	//reset position, orientation and joint angles 
	m_rootNode->reset();
}

void A3::resetJoints(){
	resetCmds();
	//reset position, orientation and joint angles 
	m_rootNode->init_joint_angle();
}

void A3::resetPosition(){
	//reset position
	m_rootNode->reset_position();
}

void A3::resetRotation(){
	//reset joint angles 
	m_rootNode->reset_rotation();
}
//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{
	delete m_picker;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

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
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	m_picker = new Picker(m_rootNode.get());
	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	// build parent hierarchy
	m_rootNode->add_parent();
	// init joint initial angle
	m_rootNode->init_joint_angle(true);
	m_rootNode->parent = nullptr;

	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("Phong.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("Phong.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
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

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
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

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
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

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(10.0f, 10.0f, 10.0f);
	m_light.rgbIntensity = vec3(1.0f,1.0f, 1.0f); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		//-- Set picking flag
		{	
			location = m_shader.getUniformLocation("ispicking");
			//std::cout << "is picking " << m_picker->is_picking << std::endl;
			glUniform1i(location, m_picker->is_picking ? 1 : 0);
			CHECK_GL_ERRORS;
		}

		if (!m_picker->is_picking){
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
				vec3 ambientIntensity(0.25f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}	// if
		

	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
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
	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
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

		// Edit panel
		if (ImGui::BeginMenu("Edit")){
			// Create Button, and check if it was clicked:
			if( ImGui::Button( "Undo (U)" ) ) {
				undo();
			}
			if( ImGui::Button( "Redo (R)" ) ) {
				redo();
			}
			ImGui::EndMenu();
		}	// if

		// Option panel
		if (ImGui::BeginMenu("Option")){
			// Create Button, and check if it was clicked:
			if (ImGui::Checkbox("Circle (C)", &m_circle)){}
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

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		bool is_picking
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix* node.trans;
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
		if (is_picking){
			kd = node.uniq_color;
		}	else {
			kd = node.material.kd;
		}
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	if (m_z_buffer){
		glEnable( GL_DEPTH_TEST );
	}
	m_rootNode.get()->accept(*this);
	if (m_z_buffer){
		glDisable( GL_DEPTH_TEST );
	}
	if (m_circle){	// draw circle if enabled
		renderArcCircle();
	}
	
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

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

		updateShaderUniforms(m_shader, *geometryNode, m_view, m_picker->is_picking);


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
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

void A3::rotate_all_selected_joints(double xAngle, double yAngle){
	for (auto& n : selected_joints){
		//std::cout << " node is " << *n.first << std::endl;
		n.first->rotate_joint(xAngle*20, yAngle*20);
	}
	//std::cout << "done one " << std::endl;
}
//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if(m_left_click || m_middle_click || m_right_click){
			double x_diff = (xPos - m_prev_x)/POS_SCALE;
			double y_diff = (yPos - m_prev_y)/POS_SCALE;
			switch (m_option_model)
			{
			case ModelPosition:
				if (m_left_click){
					m_rootNode->applyTranslate(vec3(x_diff, -y_diff, 0));
				}	 
				if (m_middle_click){
					m_rootNode->applyTranslate(vec3(0, 0, y_diff));
				}
				if (m_right_click){
					trackball(m_prev_x, xPos, m_prev_y, yPos);
				}
				break;
			case ModelJoints:
				if (m_right_click){
					rotate_all_selected_joints(y_diff, y_diff);
				}
				if (m_right_click){
					if (m_headNode) m_headNode->rotate_joint(y_diff*20, y_diff*20);
				}
				break;
			default:
				abort();
			}
		}
		// update prev x
		m_prev_x = xPos;
		m_prev_y = yPos;
	}

	return eventHandled;
}

void A3::processPicking(){
	m_picker->is_picking = false;
	double xpos, ypos;
	get_cursor_pos(xpos, ypos);
	GeometryNode* res = m_picker->pick(xpos, ypos);
	if (res != nullptr){
		JointNode* joint = res->get_upper_joint();
		if (joint != nullptr){
			if (res->select()){
				if (res->m_name == "head"){
					m_headNode = joint;
				}
				selected_joints[joint] = make_pair(0.0f, 0.0f);
			}	else {
				if (res->m_name == "head"){
					m_headNode = nullptr;
				}
				cout << " in init joint node " << joint->m_name << " has quat " << glm::quat_cast(joint->jointRotationM)<<  " has rot " << joint->jointRotationM <<  "  w is " <<glm::quat_cast(joint->jointRotationM).w << "  x is " <<glm::quat_cast(joint->jointRotationM).x << "  y is " <<glm::quat_cast(joint->jointRotationM).y << "  z is " <<glm::quat_cast(joint->jointRotationM).z  << std::endl;
				selected_joints.erase(joint);
			}
		}	// if
	}	// if
}	// process picking

void A3::push_command(){
	std::unordered_map<JointNode*, std::pair<float, float>> saved_nodes;
	for (auto& node : selected_joints){
		saved_nodes[node.first] = make_pair(node.first->m_angleX - node.second.first, node.first->m_angleY - node.second.second);
		//std::cout << *node.first << " x angle offset is " << saved_nodes[node.first].first << " y angle offset is " << saved_nodes[node.first].second << std::endl;
	}
	if (m_command_idx != commands.size()-1){	// if performed undo and 
		commands.clear();
		m_command_idx = -1;
	}
	commands.emplace_back(std::move(saved_nodes));
	m_command_idx++;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);
	bool middle_press, right_press;
	middle_press = right_press = false;
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window. 
		if (actions == GLFW_PRESS){
		
			if (button == GLFW_MOUSE_BUTTON_LEFT){
				// if left button pressed and joint model, init picking
				if (m_option_model == ModelJoints){
					m_picker->is_picking = true;
				}
				m_left_click = true;
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE){
				m_middle_click = true;
				middle_press = true;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT){
				right_press = true;
				m_right_click = true;
			}

			if (m_option_model == ModelJoints && (middle_press || right_press)){	// if first click, save state
				//m_selected_prev_m = m_selected_node->get_transform();
				save_selected_joints();
			}

			
			eventHandled = true;
		}	// if
	}	// if
	bool middle_release, right_release;
	middle_release = right_release = false;
	// if release the mouse, cancel signal
	if (actions == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_LEFT){
			// if left button pressed and joint model, cancel picking
			if (m_option_model == ModelJoints){
				processPicking();	
			}
			m_left_click = false;
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE){
			m_middle_click = false;
			middle_release = true;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT){
			m_right_click = false;
			right_release = true;
		}
		if (m_option_model == ModelJoints && (middle_release || right_release)){	// if first release, push one command
			push_command();
		}
		eventHandled = true;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		if (key == GLFW_KEY_P){
			m_option_model = ModelPosition;
			eventHandled = true;
		}
		if (key == GLFW_KEY_J){
			m_option_model = ModelJoints;
			eventHandled = true;
		}
		if (key == GLFW_KEY_Q){
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_U){
			undo();
			eventHandled = true;
		}
		if (key == GLFW_KEY_R){
			redo();
			eventHandled = true;
		}
		if (key == GLFW_KEY_Z){
			m_z_buffer = !m_z_buffer;
			eventHandled = true;
		}
		if (key == GLFW_KEY_C){
			m_circle = !m_circle;
			eventHandled = true;
		}
		if (key == GLFW_KEY_B){
			m_backface = !m_backface;
			eventHandled = true;
		}
		if (key == GLFW_KEY_F){
			m_frontface = !m_frontface;
			eventHandled = true;
		}
		if (key == GLFW_KEY_I){
			resetPosition();
			eventHandled = true;
		}
		if (key == GLFW_KEY_O){
			resetRotation();
			eventHandled = true;
		}
		if (key == GLFW_KEY_S){
			resetJoints();
			eventHandled = true;
		}
		if (key == GLFW_KEY_A){
			resetAll();
			eventHandled = true;
		}


	}
	// Fill in with event handling code...

	return eventHandled;
}
