#pragma once

#ifndef _MR_CONFIG_H_
#define _MR_CONFIG_H_

/** COMPILE TIME CONFIG **/
#define GLEW_STATIC
#define GLM_FORCE_CXX11
#define MR_ASYNC_LOADING_DEFAULT false
#define MR_RESOURCE_MANAGER_DEBUG_MSG_DEFAULT false
//#define MR_GEOMETRY_DEBUG //undefine it, to turn off logging gl commands
//#define MR_DEBUG_LOG_CAMERA

/** COMPILE TIME ENGINE CONSTANTS **/
#define MR_DELTA_TYPE float

/** DEBUG **/
//#define MR_CHECK_SMALL_GL_ERRORS ///undefine it, to turn off
#define MR_CHECK_LARGE_GL_ERRORS ///undefine it, to turn off

//Textures
#define MR_DEFAULT_TEXTURE_SETTINGS_LOD_BIAS        0.0f
#define MR_DEFAULT_TEXTURE_SETTINGS_BORDER_COLOR    0.0f, 0.0f, 0.0f, 0.0f
#define MR_DEFAULT_TEXTURE_SETTINGS_MAG_FILTER      MagFilter_LINEAR
#define MR_DEFAULT_TEXTURE_SETTINGS_MIN_FILTER      GL_LINEAR_MIPMAP_NEAREST
#define MR_DEFAULT_TEXTURE_SETTINGS_MIN_LOD         -1000
#define MR_DEFAULT_TEXTURE_SETTINGS_MAX_LOD         1000
#define MR_DEFAULT_TEXTURE_SETTINGS_WRAP_S          GL_CLAMP_TO_EDGE
#define MR_DEFAULT_TEXTURE_SETTINGS_WRAP_R          GL_CLAMP_TO_EDGE
#define MR_DEFAULT_TEXTURE_SETTINGS_WRAP_T          GL_CLAMP_TO_EDGE
#define MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_MODE    CompareMode_NONE
#define MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_FUNC    CompareFunc_LEQUAL
#define MR_TEXTURE_DIFFUSE_STAGE                    GL_TEXTURE0

#endif // _MR_CONFIG_H_
