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
#include "Utils/ConfigClass.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Entity.hpp"
#include "Utils/Events.hpp"
#include "Geometry/GeometryBufferV2.hpp"
#include "Utils/Log.hpp"
#include "Scene/Light.hpp"
#include "MachineInfo.hpp"
#include "Material.hpp"
#include "Geometry/Mesh.hpp"
#include "Model.hpp"
#include "Scene/Scene.hpp"
#include "Shaders/ShaderInterfaces.hpp"
#include "RenderSystem.hpp"
#include "RenderTarget.hpp"
#include "RenderWindow.hpp"
#include "Resources/ResourceManager.hpp"
#include "Texture.hpp"
#include "Scene/Transformation.hpp"
#include "Utils/Time.hpp"
#include "UI.hpp"

#endif // _MR_MORGLODS_RENDER_H_
