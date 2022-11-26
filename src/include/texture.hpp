#pragma once

#include <string>
#include <vector>
#include "cs488-framework/OpenGLImport.hpp"

class Texture {
	std::vector<unsigned char> m_rawdata;
	unsigned m_width, m_height;
	bool m_initialized;
	std::string m_filePath;
public: 
	GLuint texturePtr;
	Texture(const std::string& filePath);

	// Sets up openGL texture and stuff; defer this until after some other setups are finished
	void initialize(const std::string& assetsDir = "Assets/textures/");
};


class SkyBox{
	std::vector<unsigned int> m_width;
    std::vector<unsigned> m_height;
	bool m_initialized;
    std::vector<std::string> m_faces;
public:
    GLuint texturePtr;
    SkyBox(const std::vector<std::string>& faces);
    void initialize(const std::string& assetsDir = "Assets/textures/");
};