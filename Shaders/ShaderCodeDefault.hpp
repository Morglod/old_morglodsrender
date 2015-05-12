#pragma once
#include <mu/Macro.hpp>

/// RUN 'build_shader_resources.py' to update this

namespace mr {

//BEGIN FILE TO CHAR "BaseShaderVS.glsl"

const char* BaseShaderVS_glsl = "#version 330\n#extension GL_ARB_separate_shader_objects : enable\n#pragma optimize (on)\n#pragma optionNV(fastmath on)\n#pragma optionNV(fastprecision on)\n#pragma optionNV(ifcvt none)\n#pragma optionNV(inline all)\n#pragma optionNV(strict on)\n#pragma optionNV(unroll all)\nprecision mediump float;\n\nlayout (location = 0) in vec3 MR_VERTEX_POSITION_ATTRIBUTE;\nlayout (location = 1) in vec2 MR_VERTEX_TEXCOORD_ATTRIBUTE;\nlayout (location = 2) in vec3 MR_VERTEX_NORMAL_ATTRIBUTE;\nlayout (location = 3) in vec4 MR_VERTEX_COLOR_ATTRIBUTE;\nlayout (location = 4) in vec3 MR_VERTEX_INSTANCED_POSITION_ATTRIBUTE;\n\nuniform mat4 MR_MAT_MVP;\nuniform mat4 MR_MAT_MODEL;\nuniform mat4 MR_MAT_VIEW;\nuniform mat4 MR_MAT_PROJ;\n\nout vec4 MR_VertexPos;\nout vec4 MR_LocalVertexPos;\nsmooth out vec3 MR_VertexNormal;\nout vec4 MR_VertexColor;\nout vec2 MR_VertexTexCoord;\nout vec3 MR_VertexInstancedPos;\n\nvoid main() {\n//TODO\n	vec4 pos = MR_MAT_MVP * MR_MAT_MODEL * vec4(MR_VERTEX_POSITION_ATTRIBUTE + MR_VERTEX_INSTANCED_POSITION_ATTRIBUTE, 1);\n	MR_VertexPos = pos;\n   MR_LocalVertexPos = vec4(MR_VERTEX_POSITION_ATTRIBUTE,1);\n   MR_VertexNormal = MR_VERTEX_NORMAL_ATTRIBUTE;\n	MR_VertexColor = MR_VERTEX_COLOR_ATTRIBUTE;\n	MR_VertexTexCoord = MR_VERTEX_TEXCOORD_ATTRIBUTE;\n	MR_VertexInstancedPos = MR_VERTEX_INSTANCED_POSITION_ATTRIBUTE;\n	gl_Position = pos;\n}\n";

//END FILE TO CHAR

//BEGIN FILE TO CHAR "BaseShaderFS.glsl"

const char* BaseShaderFS_glsl = "#version 400\n#extension GL_ARB_uniform_buffer_object : require\n#extension GL_ARB_bindless_texture : require\n\n#define EPSILON 0.0000001\n\n#define MAX_POINT_LIGHTS 100\n#define MAX_TEXTURES 1000\n\n//precision mediump float;\n\nin vec4 MR_VertexPos;\nin vec4 MR_LocalVertexPos;\nsmooth in vec3 MR_VertexNormal;\nin vec4 MR_VertexColor;\nin vec2 MR_VertexTexCoord;\nin vec3 MR_VertexInstancedPos;\n\nuniform vec3 MR_CAM_POS;\nuniform vec3 MR_CAM_DIR;\n\nuniform mat4 MR_MAT_MVP;\nuniform mat4 MR_MAT_MODEL;\nuniform mat4 MR_MAT_VIEW;\nuniform mat4 MR_MAT_PROJ;\n\nuniform int MR_TEX_COLOR;\n\nlayout(std140) uniform MR_Textures_Block {\n    sampler2D MR_textures[MAX_TEXTURES];\n};\n\nuniform vec4 MR_MATERIAL_COLOR;\n\nout vec4 MR_fragSceneColorNothing;\n\nstruct MR_PointLight {\n   vec3 pos;\n   vec3 color;\n   float innerRange;\n   float outerRange;\n};\n\nuniform int MR_numPointLights;\n\nlayout(std140) uniform MR_pointLights_block\n{\n    MR_PointLight MR_pointLights[MAX_POINT_LIGHTS];\n};\n\nvec2 SphericalTexCoord(in vec3 normal) {\n    float m = 2.0 * sqrt(\n        normal.x*normal.x +\n        normal.y*normal.y +\n        (normal.z + 1.0)*(normal.z + 1.0)\n    );\n    return normal.xy / (m + EPSILON) + vec2(0.5);\n}\n\nvoid GeometryClipSphere(in vec3 pos, in float r) {\n    vec3 surfPos = (MR_MAT_MODEL * MR_LocalVertexPos).xyz + MR_VertexInstancedPos;\n    float inv_dist = 1.0 / (length(surfPos - pos) + EPSILON);\n    float inv_r = 1.0 / r;\n    float il = max(inv_dist, inv_r) - inv_r;\n    il = il / (il + EPSILON); // 1.0 or 0.0\n    if(il <= 0.5) discard;\n}\n\nfloat ZeroOrOne(in float value) {\n    return value / (value + EPSILON);\n}\n\nfloat ScalarInterp(in float x1, in float x2, in float x) {\n    return (x-x1) / (x2-x1 + EPSILON);\n}\n\nvec3 ApplyPointLights(in vec3 surfaceColor, in vec3 surfaceNormal) {\n    vec3 result = vec3(0,0,0);\n    vec3 surfPos = (MR_MAT_MODEL * MR_LocalVertexPos).xyz + MR_VertexInstancedPos;\n    for(int i = 0; i < MR_numPointLights; i++){\n        float dist = length(surfPos - MR_pointLights[i].pos);\n        float inv_dist = 1.0 / (dist + EPSILON);\n        float inv_innerR = 1.0 / MR_pointLights[i].innerRange;\n        float il = max(inv_dist, inv_innerR) - inv_innerR;\n        il = ZeroOrOne(il); // 1.0 or 0.0\n        float al = 1.0 - ScalarInterp(MR_pointLights[i].innerRange, MR_pointLights[i].outerRange, dist);\n        result += MR_pointLights[i].color * clamp(il + al, 0.0, 1.0);\n    }\n    return result * surfaceColor;\n}\n\nvec3 ApplyLights(in vec3 surfaceColor, in vec3 surfaceNormal) {\n    return ApplyPointLights(surfaceColor, surfaceNormal);\n}\n\nvoid main() {\n    vec4 surface_normal = normalize((vec4(MR_VertexNormal,0) * MR_MAT_MODEL));\n    vec3 albedoColor = ApplyLights(texture(MR_textures[MR_TEX_COLOR], MR_VertexTexCoord).xyz, surface_normal.xyz);\n    MR_fragSceneColorNothing = vec4(albedoColor, 1.0);\n}\n";

//END FILE TO CHAR

//This shader code may be changed during runtime
std::string BaseShaderVS_glsl_s = BaseShaderVS_glsl;
std::string BaseShaderFS_glsl_s = BaseShaderFS_glsl;

}
