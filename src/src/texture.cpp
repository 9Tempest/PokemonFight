#include "texture.hpp"
#include "debug.hpp"
#include "lodepng/lodepng.h"
#include "lodepng/lodepng_util.h"
#include "cs488-framework/GlErrorCheck.hpp"
#include "include.hpp"

using namespace std;

Texture::Texture(const std::string& filePath): m_initialized(false), m_filePath(filePath){}




SkyBox::SkyBox(const std::vector<std::string>& faces):  m_initialized(false), m_faces(faces) {

}

std::unordered_map<std::string, Texture*> TextureAssets::assets;

void TextureAssets::initialize(){
    for (auto& p : assets){
        p.second->initialize();
    }
}

void Texture::initialize(const std::string& assetsDir){
    if (m_initialized){
        DLOG("Should be initialized once; but found twice");
        return;
    }
    std::vector<unsigned char> m_rawdata;
    unsigned error = lodepng::decode(m_rawdata, m_width, m_height, assetsDir + m_filePath, LCT_RGBA);
    if (error) {
        DLOG("decoder error %u : %s", error, lodepng_error_text(error));
        abort();
        return;
    }
    // Generate opengl texture to texturePtr
    glGenTextures(1, &texturePtr);
    glBindTexture(GL_TEXTURE_2D, texturePtr);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CHECK_GL_ERRORS;

    // Put the raw data into the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_rawdata.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    CHECK_GL_ERRORS;

    // Unbind 
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERRORS;
}


void SkyBox::initialize(const std::string& assetsDir){
    if (m_initialized){
        DLOG("Should be initialized once; but found twice");
        return;
    }
    assert(m_faces.size() == 6);
    CHECK_GL_ERRORS;
    m_width.resize(m_faces.size());
    m_height.resize(m_faces.size());
    glGenTextures(1, &texturePtr);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texturePtr);
    for (uint i = 0; i < m_faces.size(); i++){
        vector<unsigned char> data;
        unsigned error = lodepng::decode(data, m_width[i], m_height[i], assetsDir + m_faces[i], LCT_RGBA);
        if (error) {
            DLOG("decoder error %u : %s", error, lodepng_error_text(error));
            abort();
            return;
        }   else {
            CHECK_GL_ERRORS;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                            0, GL_RGBA, m_width[i], m_height[i], 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()
                );
            CHECK_GL_ERRORS;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // unbind
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    CHECK_GL_ERRORS;
    DLOG("Done initialize sky box!");
    m_initialized = true;
}
