#pragma once

/** 3rd party libs **/
#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

#ifndef glm_glm
#   define GLM_FORCE_CXX11
//#   define GLM_FORCE_INLINE
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#   include <glm/gtc/type_ptr.hpp>
#endif

#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#   include <SOIL.h>
#endif

/** std libs **/
#include <memory>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <locale>

/** config **/
#define MR_DELTA_TYPE float
#define MR_DEFAULT_TEXTURE_SETTINGS_LOD_BIAS 0.0f
#define MR_DEFAULT_TEXTURE_SETTINGS_BORDER_COLOR 0.0f, 0.0f, 0.0f, 0.0f
#define MR_DEFAULT_TEXTURE_SETTINGS_MAG_FILTER GL_LINEAR
#define MR_DEFAULT_TEXTURE_SETTINGS_MIN_FILTER GL_LINEAR
#define MR_DEFAULT_TEXTURE_SETTINGS_MIN_LOD -1000
#define MR_DEFAULT_TEXTURE_SETTINGS_MAX_LOD 1000
#define MR_DEFAULT_TEXTURE_SETTINGS_WRAP_S GL_REPEAT
#define MR_DEFAULT_TEXTURE_SETTINGS_WRAP_R GL_REPEAT
#define MR_DEFAULT_TEXTURE_SETTINGS_WRAP_T GL_REPEAT
#define MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_MODE GL_NONE
#define MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_FUNC GL_LEQUAL

/** interfaces **/
template<typename T>
class Copyable {
public:
    virtual T* Copy() = 0;
};
