/*
MorglodsRender library header.
-Render framework, uses OpenGL.

3rd:
OpenGL
GLFW http://www.glfw.org/
SOIL (Simple Image Library) http://www.lonesock.net/soil.html
Assimp For MoConverter Only
GLM http://glm.g-truc.net/0.9.5/index.html
GLEW GCC REBUILD http://glew.sourceforge.net/
zlib http://www.zlib.net
*/

#pragma once

#ifndef _MR_MORGLODS_RENDER_H_
#define _MR_MORGLODS_RENDER_H_

#include "Config.hpp"
#include "Scene/PerspectiveCamera.hpp"
//#include "Scene/Entity.hpp"
#include "Utils/Events.hpp"
#include "Geometry/GeometryBuffer.hpp"
#include "Utils/Log.hpp"
//#include "Scene/Light.hpp"
#include "MachineInfo.hpp"
#include "Materials/MaterialInterfaces.hpp"
#include "Geometry/Mesh.hpp"
//#include "Model.hpp"
//#include "Scene/Scene.hpp"
#include "Shaders/ShaderInterfaces.hpp"
#include "Shaders/ShaderCompiler.hpp"
#include "Shaders/ShaderUniforms.hpp"
#include "Shaders/ShaderObjects.hpp"
#include "Core.hpp"
#include "Textures/TextureInterfaces.hpp"
#include "Resources/ResourceManager.hpp"
#include "Scene/Transformation.hpp"
#include "Query.hpp"

#endif // _MR_MORGLODS_RENDER_H_
