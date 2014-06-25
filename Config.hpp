#pragma once

#ifndef _MR_CONFIG_H_
#define _MR_CONFIG_H_

/** COMPILE TIME CONFIG **/
#define GLEW_STATIC
#define GLM_FORCE_CXX11
#define MR_ASYNC_LOADING_DEFAULT false//true
#define MR_ASYNC_ONE_RES_PER_UPDATE_LOADING_DEFAULT false//true
#define MR_RESOURCE_MANAGER_DEBUG_MSG_DEFAULT false

/** COMPILE TIME ENGINE CONSTANTS **/
#define MR_DELTA_TYPE float

/** DEBUG **/
#define MR_CHECK_SMALL_GL_ERRORS ///undefine it, to turn off
#define MR_CHECK_LARGE_GL_ERRORS ///undefine it, to turn off

//Textures
#define MR_DEFAULT_TEXTURE_SETTINGS_LOD_BIAS        0.0f
#define MR_DEFAULT_TEXTURE_SETTINGS_BORDER_COLOR    0.0f, 0.0f, 0.0f, 0.0f
#define MR_DEFAULT_TEXTURE_SETTINGS_MAG_FILTER      MagFilter::LINEAR
#define MR_DEFAULT_TEXTURE_SETTINGS_MIN_FILTER      GL_LINEAR_MIPMAP_NEAREST
#define MR_DEFAULT_TEXTURE_SETTINGS_MIN_LOD         -1000
#define MR_DEFAULT_TEXTURE_SETTINGS_MAX_LOD         1000
#define MR_DEFAULT_TEXTURE_SETTINGS_WRAP_S          Wrap::CLAMP
#define MR_DEFAULT_TEXTURE_SETTINGS_WRAP_R          Wrap::CLAMP
#define MR_DEFAULT_TEXTURE_SETTINGS_WRAP_T          Wrap::CLAMP
#define MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_MODE    CompareMode::NONE
#define MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_FUNC    CompareFunc::LEQUAL
#define MR_TEXTURE_DIFFUSE_STAGE                    GL_TEXTURE0

//Shader attributes
#define MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION   0
#define MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION     1
#define MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION      2
#define MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION   3

#define MR_SHADER_VERTEX_POSITION_ATTRIB_NAME   "VERTEX_POSITION"
#define MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME     "VERTEX_NORMAL"
#define MR_SHADER_VERTEX_COLOR_ATTRIB_NAME      "VERTEX_COLOR"
#define MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME   "VERTEX_TEXCOORD"

#define MR_SHADER_DEFAULT_FRAG_DATA_LOCATION    0
#define MR_SHADER_DEFAULT_FRAG_DATA_NAME        "fragColor"

#define MR_SHADER_DEFAULT_DEPTH_TEX_COORD_DATA_LOCATION   1
#define MR_SHADER_DEFAULT_DEPTH_TEX_COORD_DATA_NAME       "fragDepthTexCoord"

#define MR_SHADER_DEFAULT_NORMAL_DATA_LOCATION   2
#define MR_SHADER_DEFAULT_NORMAL_DATA_NAME       "fragNormal"

#define MR_SHADER_DEFAULT_POS_DATA_LOCATION      3
#define MR_SHADER_DEFAULT_POS_DATA_NAME          "fragPos"

#define MR_SHADER_UNIFORM_GBUFFER_ALBEDO            "GBUFFER_ALBEDO"
#define MR_SHADER_UNIFORM_GBUFFER_DEPT_TEX_COORD    "GBUFFER_DEPT_TEX_COORD"
#define MR_SHADER_UNIFORM_GBUFFER_NORMAL            "GBUFFER_NORMAL"
#define MR_SHADER_UNIFORM_GBUFFER_POS               "GBUFFER_POS"

//Shader uniforms
#define MR_SHADER_MVP_MAT4      "MAT_MVP"
#define MR_SHADER_MODEL_MAT4    "MAT_MODEL"
#define MR_SHADER_VIEW_MAT4     "MAT_VIEW"
#define MR_SHADER_PROJ_MAT4     "MAT_PROJ"

#define MR_SHADER_CAM_POS       "CAM_POS"
#define MR_SHADER_CAM_DIR       "CAM_DIR"

#define MR_SHADER_AMBIENT_TEX       "ENGINE_AMBIENT_MAP"
#define MR_SHADER_DIFFUSE_TEX       "ENGINE_DIFFUSE_MAP"
#define MR_SHADER_EMISSIVE_TEX      "ENGINE_EMISSIVE"
#define MR_SHADER_ENVIRONMENT_TEX   "ENGINE_ENVIRONMENT"
#define MR_SHADER_ALPHA_DISCARD_F   "ENGINE_ALPHA_DISCARD"
#define MR_SHADER_COLOR_V4          "ENGINE_COLOR"
#define MR_SHADER_EMISSIVE_V4       "ENGINE_EMISSIVE"
#define MR_SHADER_OPACITY_TEX       "ENGINE_OPACITY_MAP"

#define MR_SHADER_MAX_POINT_LIGHTS  20
#define MR_SHADER_POINT_LIGHTS_NUM  "SCENE_PointLightsNum"
#define MR_SHADER_POINT_LIGHTS      "SCENE_PointLights"

#define MR_SHADER_MAX_DIR_LIGHTS    10
#define MR_SHADER_DIR_LIGHTS_NUM    "SCENE_DirLightsNum"
#define MR_SHADER_DIR_LIGHTS        "SCENE_DirLights"

#define MR_SHADER_FOG_MAX_DIST      "SCENE_FogMaxDist"
#define MR_SHADER_FOG_MIN_DIST      "SCENE_FogMinDist"
#define MR_SHADER_FOG_COLOR         "SCENE_FogColor"

//Material default values
#define MR_MATERIAL_AMBIENT_TEX_DEFAULT     10
#define MR_MATERIAL_DIFFUSE_TEX_DEFAULT     11
#define MR_MATERIAL_OPACITY_TEX_DEFAULT     12
#define MR_MATERIAL_ENVIRONMENT_TEX_DEFAULT 10
#define MR_MATERIAL_ALPHA_DISCARD_F_DEFAULT 0.5f
#define MR_MATERIAL_COLOR_V4_DEFAULT        1,1,1,1
#define MR_MATERIAL_EMISSIVE_V4_DEFAULT     0,0,0,0

#endif // _MR_CONFIG_H_
