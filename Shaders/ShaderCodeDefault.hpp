#pragma once
#include <Macro.hpp>

/// RUN 'build_shader_resources.py' to update this

namespace mr {

//BEGIN FILE TO CHAR "BaseShaderVS.glsl"

const char* BaseShaderVS_glsl = "#version 330 \n#extension GL_ARB_separate_shader_objects : enable\n#pragma optimize (on)\n#pragma optionNV(fastmath on)\n#pragma optionNV(fastprecision on)\n#pragma optionNV(ifcvt none)\n#pragma optionNV(inline all)\n#pragma optionNV(strict on)\n#pragma optionNV(unroll all)\nprecision mediump float;\n\nlayout (location = 0) in vec3 MR_VERTEX_POSITION_ATTRIBUTE;\nlayout (location = 1) in vec2 MR_VERTEX_TEXCOORD_ATTRIBUTE;\nlayout (location = 2) in vec3 MR_VERTEX_NORMAL_ATTRIBUTE;\nlayout (location = 3) in vec4 MR_VERTEX_COLOR_ATTRIBUTE;\n//TODO\nlayout (location = 4) in vec3 _TEST_INST;\n\nuniform mat4 MR_MAT_MVP;\nuniform mat4 MR_MAT_MODEL;\nuniform mat4 MR_MAT_VIEW;\nuniform mat4 MR_MAT_PROJ;\n\nout vec4 MR_VertexPos;\nout vec4 MR_LocalVertexPos;\nsmooth out vec3 MR_VertexNormal;\nout vec4 MR_VertexColor;\nout vec2 MR_VertexTexCoord;\n\nvoid main() {\n//TODO\n	vec4 pos = MR_MAT_MVP * MR_MAT_MODEL * vec4(MR_VERTEX_POSITION_ATTRIBUTE + _TEST_INST, 1);\n	MR_VertexPos = pos;\n   MR_LocalVertexPos = vec4(MR_VERTEX_POSITION_ATTRIBUTE,1);\n   MR_VertexNormal = MR_VERTEX_NORMAL_ATTRIBUTE;\n	MR_VertexColor = MR_VERTEX_COLOR_ATTRIBUTE;\n	MR_VertexTexCoord = MR_VERTEX_TEXCOORD_ATTRIBUTE;\n	gl_Position = pos;\n}";

//END FILE TO CHAR

//BEGIN FILE TO CHAR "BaseShaderFS.glsl"

const char* BaseShaderFS_glsl = "#version 330 \n#extension GL_ARB_separate_shader_objects : enable\n#pragma optimize (on)\n#pragma optionNV(fastmath on)\n#pragma optionNV(fastprecision on)\n#pragma optionNV(ifcvt none)\n#pragma optionNV(inline all)\n#pragma optionNV(strict on)\n#pragma optionNV(unroll all)\nprecision mediump float;\n\nin vec4 MR_VertexPos;\nin vec4 MR_LocalVertexPos;\nsmooth in vec3 MR_VertexNormal;\nin vec4 MR_VertexColor;\nin vec2 MR_VertexTexCoord;\n\nuniform vec3 MR_CAM_POS;\nuniform vec3 MR_CAM_DIR;\n\nuniform mat4 MR_MAT_MVP;\nuniform mat4 MR_MAT_MODEL;\nuniform mat4 MR_MAT_VIEW;\nuniform mat4 MR_MAT_PROJ;\n\nuniform sampler2D MR_TEX_COLOR;\nuniform vec4 MR_MATERIAL_COLOR;\n\nout vec4 MR_fragSceneColorNothing;\nconst vec3 lightSunDir = vec3(-0.5, -1, -0.5);\nvoid main() {\n   vec3 albedoColor = texture(MR_TEX_COLOR, MR_VertexTexCoord).xyz;\n   vec3 lightNormal = normalize(lightSunDir);\n   vec3 lightFactor = normalize((vec4(MR_VertexNormal,0) * MR_MAT_MODEL).xyz);\n   float lightF = dot(-lightNormal, lightFactor) * 0.3;\n   float ambient = 0.5;\n// * lightF * dot(vec4(-1 * MR_VertexNormal, 0),\n   MR_fragSceneColorNothing = vec4(albedoColor*lightF + albedoColor*ambient, 1.0);\n}";

//END FILE TO CHAR

}
