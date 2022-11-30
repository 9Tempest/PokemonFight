#pragma once
#include "include.hpp"
const unsigned int SHADOW_WIDTH = 1200, SHADOW_HEIGHT = 1200;

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


struct ShadowMap{
    unsigned int depthMapFBO;
    unsigned int depthMap;
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    glm::mat4 lightSpaceMatrix;
    const float near_plane = 0.0f;
    const float far_plane = 100.0f;
    LightSource* light;

    void init(LightSource& src);

};
