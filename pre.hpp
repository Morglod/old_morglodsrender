#pragma once

//GLEW
#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

//GLFW
#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

//GLM
#ifndef glm_glm
#   define GLM_FORCE_CXX11
//#   define GLM_FORCE_INLINE
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#   include <glm/gtc/type_ptr.hpp>
#endif

//SIMPLE IMAGE LIBRARY
#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#   include <SOIL.h>
#endif

#include <memory>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>

//ASSIMP
/*#ifndef AI_ASSIMP_H_INC
#   include <assimp/cimport.h>
#endif
#ifndef AI_MATERIAL_H_INC
#   include <assimp/material.h>
#endif
#ifndef __AI_SCENE_H_INC__
#   include <assimp/scene.h>
#endif
#ifndef AI_POSTPROCESS_H_INC
#   include <assimp/postprocess.h>
#endif
#ifndef INCLUDED_AI_ASSIMP_HPP
#   include <assimp/Importer.hpp>
#endif
#ifndef INCLUDED_AI_MESH_H
#   include <assimp/mesh.h>
#endif*/
