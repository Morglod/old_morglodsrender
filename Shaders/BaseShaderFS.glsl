#version 400
#extension GL_ARB_bindless_texture : require
#extension GL_NV_gpu_shader5 : enable

#define EPSILON 0.0000001

#define MAX_POINT_LIGHTS 100
#define MAX_TEXTURES 1000

//precision mediump float;

in vec4 MR_VertexPos;
in vec4 MR_LocalVertexPos;
smooth in vec3 MR_VertexNormal;
in vec4 MR_VertexColor;
in vec2 MR_VertexTexCoord;
in vec3 MR_VertexInstancedPos;

uniform vec3 MR_CAM_POS;
uniform vec3 MR_CAM_DIR;

uniform mat4 MR_MAT_MVP;
uniform mat4 MR_MAT_MODEL;
uniform mat4 MR_MAT_VIEW;
uniform mat4 MR_MAT_PROJ;

uniform unsigned int MR_TEX_COLOR;

struct MR_TextureHandle {
	uint64_t handle;
};

layout(std140) uniform MR_Textures_Block
{
    MR_TextureHandle MR_textures[MAX_TEXTURES];
};

uniform vec4 MR_MATERIAL_COLOR;

out vec4 MR_fragSceneColorNothing;

struct MR_PointLight {
   vec3 pos;
   vec3 color;
   float innerRange;
   float outerRange;
};

uniform int MR_numPointLights;

layout(std140) uniform MR_pointLights_block
{
    MR_PointLight MR_pointLights[MAX_POINT_LIGHTS];
};

vec2 SphericalTexCoord(in vec3 normal) {
    float m = 2.0 * sqrt(
        normal.x*normal.x +
        normal.y*normal.y +
        (normal.z + 1.0)*(normal.z + 1.0)
    );
    return normal.xy / (m + EPSILON) + vec2(0.5);
}

void GeometryClipSphere(in vec3 pos, in float r) {
    vec3 surfPos = (MR_MAT_MODEL * MR_LocalVertexPos).xyz + MR_VertexInstancedPos;
    float inv_dist = 1.0 / (length(surfPos - pos) + EPSILON);
    float inv_r = 1.0 / r;
    float il = max(inv_dist, inv_r) - inv_r;
    il = il / (il + EPSILON); // 1.0 or 0.0
    if(il <= 0.5) discard;
}

float ZeroOrOne(in float value) {
    return value / (value + EPSILON);
}

float ScalarInterp(in float x1, in float x2, in float x) {
    return (x-x1) / (x2-x1 + EPSILON);
}

vec3 ApplyPointLights(in vec3 surfaceColor, in vec3 surfaceNormal) {
    vec3 result = vec3(0,0,0);
    vec3 surfPos = (MR_MAT_MODEL * MR_LocalVertexPos).xyz + MR_VertexInstancedPos;
    for(int i = 0; i < MR_numPointLights; i++){
        float dist = length(surfPos - MR_pointLights[i].pos);
        float inv_dist = 1.0 / (dist + EPSILON);
        float inv_innerR = 1.0 / MR_pointLights[i].innerRange;
        float il = max(inv_dist, inv_innerR) - inv_innerR;
        il = ZeroOrOne(il); // 1.0 or 0.0
        float al = 1.0 - ScalarInterp(MR_pointLights[i].innerRange, MR_pointLights[i].outerRange, dist);
        result += MR_pointLights[i].color * clamp(il + al, 0.0, 1.0);
    }
    return result * surfaceColor;
}

vec3 ApplyLights(in vec3 surfaceColor, in vec3 surfaceNormal) {
    return ApplyPointLights(surfaceColor, surfaceNormal);
}

void main() {
    vec4 surface_normal = normalize((vec4(MR_VertexNormal,0) * MR_MAT_MODEL));
    vec3 albedoColor = ApplyLights(texture(sampler2D(MR_textures[MR_TEX_COLOR].handle), MR_VertexTexCoord).xyz, surface_normal.xyz);
    MR_fragSceneColorNothing = vec4(albedoColor, 1.0);
}
