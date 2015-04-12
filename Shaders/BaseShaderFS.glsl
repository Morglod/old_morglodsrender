#version 330
#extension GL_ARB_separate_shader_objects : enable
#pragma optimize (on)
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)
precision mediump float;

in vec4 MR_VertexPos;
in vec4 MR_LocalVertexPos;
smooth in vec3 MR_VertexNormal;
in vec4 MR_VertexColor;
in vec2 MR_VertexTexCoord;

uniform vec3 MR_CAM_POS;
uniform vec3 MR_CAM_DIR;

uniform mat4 MR_MAT_MVP;
uniform mat4 MR_MAT_MODEL;
uniform mat4 MR_MAT_VIEW;
uniform mat4 MR_MAT_PROJ;

uniform sampler2D MR_TEX_COLOR;
uniform vec4 MR_MATERIAL_COLOR;

out vec4 MR_fragSceneColorNothing;
const vec3 lightSunDir = vec3(-0.5, -1, -0.5);
void main() {
   vec3 albedoColor = texture(MR_TEX_COLOR, MR_VertexTexCoord).xyz;
   vec3 lightNormal = normalize(lightSunDir);
   vec3 lightFactor = normalize((vec4(MR_VertexNormal,0) * MR_MAT_MODEL).xyz);
   float lightF = dot(-lightNormal, lightFactor) * 0.3;
   float ambient = 0.5;
// * lightF * dot(vec4(-1 * MR_VertexNormal, 0),
   MR_fragSceneColorNothing = vec4(albedoColor*lightF + albedoColor*ambient, 1.0);
}
