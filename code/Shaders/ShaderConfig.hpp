#pragma once

#ifndef _MR_SHADER_CONFIG_H_
#define _MR_SHADER_CONFIG_H_

#include "../Config.hpp"

#define MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION   0
#define MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION   1
#define MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION     2
#define MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION      3

#define MR_SHADER_VERTEX_POSITION_ATTRIB_NAME   "MR_VERTEX_POSITION_ATTRIBUTE"
#define MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME   "MR_VERTEX_TEXCOORD_ATTRIBUTE"
#define MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME     "MR_VERTEX_NORMAL_ATTRIBUTE"
#define MR_SHADER_VERTEX_COLOR_ATTRIB_NAME      "MR_VERTEX_COLOR_ATTRIBUTE"

#define MR_SHADER_MODEL_MAT4    "MR_MAT_MODEL"
#define MR_SHADER_VIEW_MAT4     "MR_MAT_VIEW"
#define MR_SHADER_PROJ_MAT4     "MR_MAT_PROJ"

/** DONT FORGET TO ADD DEFAULT BLOCKS TO ShaderManager::CompleteShaderProgram **/
#define MR_SHADER_TEXTURE_BLOCK "MR_Textures_Block"
#define MR_SHADER_TEXTURE_BLOCK_BINDING 0

#define MR_SHADER_POINTLIGHTS_BLOCK "MR_pointLights_block"
#define MR_SHADER_POINTLIGHTS_BLOCK_BINDING 1

#endif