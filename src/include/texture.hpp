#pragma once

#include <string>
#include <vector>
#include "cs488-framework/OpenGLImport.hpp"
#include <unordered_map>
class Texture {
	unsigned m_width, m_height;
	bool m_initialized;
	std::string m_filePath;
public: 
	const std::string& get_name() const{return m_filePath;}
	GLuint texturePtr;
	Texture() = default;
	Texture(const std::string& filePath);
	Texture(const Texture& other) = default;
	Texture(Texture&& other) = default;
	Texture& operator=(const Texture& other) = default;
	Texture& operator=(Texture&& other) = default;

	// Sets up openGL texture and stuff; defer this until after some other setups are finished
	void initialize(const std::string& assetsDir = "Assets/textures/");
};

class TextureAssets{
	static std::unordered_map<std::string, Texture*> assets;
	public:
		static Texture* get_asset(const std::string& name) {
			if (assets.find(name) == assets.end()) return nullptr;
			return assets[name];
		}

		static void set_asset(const std::string& name, Texture* other){
			assets[name] = other;
		}

		static void initialize();
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