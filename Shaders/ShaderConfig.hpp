#pragma once

#ifndef _MR_SHADER_CONFIG_H_
#define _MR_SHADER_CONFIG_H_

#include "../Config.hpp"

//
#define MR_SHADER_BUILDER_CODE_CACHE_SIZE 10

//Shader attributes
#define MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION   0
#define MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION     1
#define MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION      2
#define MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION   3

#define MR_SHADER_VERTEX_POSITION_ATTRIB_NAME   "MR_VERTEX_POSITION_ATTRIBUTE"
#define MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME     "MR_VERTEX_NORMAL_ATTRIBUTE"
#define MR_SHADER_VERTEX_COLOR_ATTRIB_NAME      "MR_VERTEX_COLOR_ATTRIBUTE"
#define MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME   "MR_VERTEX_TEXCOORD_ATTRIBUTE"

#define MR_SHADER_DEFAULT_FRAG_DATA_LOCATION_1  0
#define MR_SHADER_DEFAULT_FRAG_DATA_NAME_1      "fragSceneColorNothing"

#define MR_SHADER_DEFAULT_FRAG_DATA_LOCATION_2  1
#define MR_SHADER_DEFAULT_FRAG_DATA_NAME_2      "fragWNormalLightModel"

#define MR_SHADER_DEFAULT_FRAG_DATA_LOCATION_3  2
#define MR_SHADER_DEFAULT_FRAG_DATA_NAME_3      "fragWPosDepth"

#define MR_SHADER_UNIFORM_GBUFFER_1             "GBUFFER_V3_SCENE_COLOR_V1_NOTH"
#define MR_SHADER_UNIFORM_GBUFFER_2             "GBUFFER_V3_WORLD_NORMAL_V1_LIGHT_MODEL"
#define MR_SHADER_UNIFORM_GBUFFER_3             "GBUFFER_V3_WORLD_POS_V1_DEPTH"

//Shader uniforms
#define MR_SHADER_MVP_MAT4      "MAT_MVP"
#define MR_SHADER_MODEL_MAT4    "MAT_MODEL"
#define MR_SHADER_VIEW_MAT4     "MAT_VIEW"
#define MR_SHADER_PROJ_MAT4     "MAT_PROJ"

#define MR_SHADER_CAM_POS       "CAM_POS"
#define MR_SHADER_CAM_DIR       "CAM_DIR"
/*
#define MR_SHADER_COLOR_MAP         "ENGINE_COLOR_MAP"
#define MR_SHADER_NORMAL_MAP        "ENGINE_NORMAL_MAP"
#define MR_SHADER_SPECULAR_MAP      "ENGINE_SPECULAR_MAP"
#define MR_SHADER_DISPLACEMENT_MAP  "ENGINE_DISPLACEMENT_MAP"
#define MR_SHADER_GLOSS_MAP         "ENGINE_GLOSS_MAP"
#define MR_SHADER_AO_MAP            "ENGINE_AO_MAP"
#define MR_SHADER_OPACITY_MAP       "ENGINE_OPACITY_MAP"
#define MR_SHADER_ENVIRONMENT_MAP   "ENGINE_ENVIRONMENT_MAP"
#define MR_SHADER_EMISSION_MAP      "ENGINE_EMISSION_MAP"
*/

#define MR_SHADER_ALBEDO_COLOR      "MR_ALBEDO_COLOR"
#define MR_SHADER_ALBEDO_TEX        "MR_ALBEDO_TEX"

#define MR_SHADER_EMISSION_COLOR    "MR_EMISSION_COLOR"

#define MR_SHADER_MAX_POINT_LIGHTS  20
#define MR_SHADER_POINT_LIGHTS_NUM  "SCENE_PointLightsNum"
#define MR_SHADER_POINT_LIGHTS      "SCENE_PointLights"

#define MR_SHADER_MAX_DIR_LIGHTS    10
#define MR_SHADER_DIR_LIGHTS_NUM    "SCENE_DirLightsNum"
#define MR_SHADER_DIR_LIGHTS        "SCENE_DirLights"

#define MR_SHADER_FOG_MAX_DIST      "SCENE_FogMaxDist"
#define MR_SHADER_FOG_MIN_DIST      "SCENE_FogMinDist"
#define MR_SHADER_FOG_COLOR         "SCENE_FogColor"

#endif // _MR_SHADER_CONFIG_H_
