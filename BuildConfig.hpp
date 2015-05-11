#pragma once

/** COMPILE TIME CONFIG **/
#define GLEW_STATIC
#define GLM_FORCE_CXX11
#define MR_ASYNC_LOADING_DEFAULT false
#define MR_RESOURCE_MANAGER_DEBUG_MSG_DEFAULT false
//#define MR_GEOMETRY_DEBUG //undefine it, to turn off logging gl commands

/** COMPILE TIME ENGINE CONSTANTS **/
#define MR_DELTA_TYPE float

/** DEBUG **/
//#define MR_CHECK_SMALL_GL_ERRORS ///undefine it, to turn off
//#define MR_CHECK_LARGE_GL_ERRORS ///undefine it, to turn off
