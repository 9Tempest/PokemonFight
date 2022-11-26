// Termm-Fall 2022

#pragma once

#include <string>
#include "SceneNode.hpp"

class SkyBox;

SceneNode * import_lua(const std::string & filename);

SceneNode * import_lua(const std::string & filename, SkyBox*& skybox_ptr);