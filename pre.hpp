#pragma once

#include "Config.hpp"

/** 3rd party libs **/
#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

#ifndef glm_glm
#   include <glm/glm.hpp>
//#   include <glm/gtc/matrix_transform.hpp>
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

/** interfaces **/
template<typename T>
class Copyable {
public:
    virtual T* Copy() = 0;
};
