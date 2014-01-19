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

#include <locale>

#define MR_DELTA_TYPE float

template<typename T>
class Copyable {
public:
    inline virtual T* Copy() {
        //assert("Copy method isn't overloaded!");
        return nullptr;
    }
};
