#version 400
#extension GL_ARB_bindless_texture : require
#extension GL_NV_gpu_shader5 : enable

/** BUILD CONFIG **/

#define MR_CUSTOM_INSTANCE_DATA 0
#define MR_UNIFORM_INSTANCE_DATA 0
#define MR_USE_ATTRIBUTE_TEXCOORD 1
#define MR_USE_ATTRIBUTE_NORMAL 1
#define MR_USE_ATTRIBUTE_COLOR 1

/** BUILD CONST **/

#define EPSILON 0.0000001

#define MAX_POINT_LIGHTS 100
#define MAX_TEXTURES 1000

/** INPUT DATA **/

in vec3 MR_VertexPos;
in vec3 MR_LocalVertexPos;

#if MR_USE_ATTRIBUTE_NORMAL == 1
smooth in vec3 MR_VertexNormal;
#endif

#if MR_USE_ATTRIBUTE_COLOR == 1
in vec4 MR_VertexColor;
#endif

#if MR_USE_ATTRIBUTE_TEXCOORD == 1
in vec2 MR_VertexTexCoord;
#endif

in vec3 MR_VertexInstancedPos;

/** PER ENTITY **/

uniform mat4 MR_MAT_MODEL;

/** CAMERA INFO **/

uniform vec3 MR_CAM_POS;
uniform mat4 MR_MAT_VIEW;
uniform mat4 MR_MAT_PROJ;

/** MATERIAL INFO **/

uniform unsigned int MR_TEX_COLOR;
uniform unsigned int MR_TEX_NORMAL;
uniform unsigned int MR_TEX_SPECULAR;
uniform float MR_TEX_NORMAL_F;
uniform float MR_TEX_SPECULAR_F;

/** TEXTURES **/

struct MR_TextureHandle {
	uint64_t handle;
};

layout(std140) uniform MR_Textures_Block
{
    MR_TextureHandle MR_textures[MAX_TEXTURES];
};

/** LIGHTS **/

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

/** OUTPUT DATA **/

out vec4 MR_fragSceneColorNothing;

/** BUILT-IN FUNCTIONS **/

vec2 SphericalTexCoord(in vec3 normal) {
    float m = 2.0 * sqrt(
        normal.x*normal.x +
        normal.y*normal.y +
        (normal.z + 1.0)*(normal.z + 1.0)
    );
    return normal.xy / (m + EPSILON) + vec2(0.5);
}

void GeometryClipSphere(in vec3 pos, in float r) {
    vec3 surfPos = (MR_MAT_MODEL * vec4(MR_LocalVertexPos,1.0)).xyz + MR_VertexInstancedPos;
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

vec3 ApplyPointLights(in vec3 surfaceColor, in vec3 surfaceDirection) {
    vec3 result = vec3(0,0,0);
    vec3 surfPos = (MR_MAT_MODEL * vec4(MR_LocalVertexPos,1.0)).xyz + MR_VertexInstancedPos;
    for(int i = 0; i < MR_numPointLights; i++){
        vec3 lightNormal = normalize(MR_pointLights[i].pos - surfPos);
        float dist = length(surfPos - MR_pointLights[i].pos);
        float inv_dist = 1.0 / (dist + EPSILON);
        float inv_innerR = 1.0 / MR_pointLights[i].innerRange;
        float innerRangeLight = max(inv_dist, inv_innerR) - inv_innerR;
        innerRangeLight = ZeroOrOne(innerRangeLight); // 1.0 or 0.0
        float outerRangeLight = 1.0 - ScalarInterp(MR_pointLights[i].innerRange, MR_pointLights[i].outerRange, dist);
        float sphereLight = clamp(innerRangeLight + outerRangeLight, 0.0, 1.0);
        //float backFaceFactor = (dot(surfaceNormal, -lightNormal) + 1.0) / 2.0;
        float resultLight = sphereLight;
        //calc normal
        vec3 normalMap = texture(sampler2D(MR_textures[MR_TEX_NORMAL].handle), MR_VertexTexCoord).rgb;
        normalMap = mix(vec3(1,1,1), normalMap, MR_TEX_NORMAL_F);
        vec3 normalMap_ = normalMap * surfaceDirection;
        vec3 normal = normalMap_;

        //Specular
        vec3 viewDir = normalize(MR_CAM_POS - surfPos);

        vec3 incidenceVector = -lightNormal; //a unit vector
        vec3 reflectionVector = reflect(incidenceVector, normal); //also a unit vector
        float cosAngle = max(0.0, dot(viewDir, reflectionVector));
        float specularCoefficient = pow(cosAngle, 3.0);
        float specularMap = mix(0.0, texture(sampler2D(MR_textures[MR_TEX_SPECULAR].handle), MR_VertexTexCoord).r, MR_TEX_SPECULAR_F);

        result += (MR_pointLights[i].color * resultLight * (max(dot(normal, lightNormal), 0.0))) + resultLight * vec3(specularMap * specularCoefficient);
    }
    return result * surfaceColor;
}

vec3 ApplyLights(in vec3 surfaceColor, in vec3 surfaceDirection) {
    return ApplyPointLights(surfaceColor, surfaceDirection);
}


/** MAIN **/

void main() {
    vec4 surface_direction = normalize((vec4(MR_VertexNormal,0) * MR_MAT_MODEL));
    vec3 albedoColor = ApplyLights(texture(sampler2D(MR_textures[MR_TEX_COLOR].handle), MR_VertexTexCoord).xyz, surface_direction.xyz);
    MR_fragSceneColorNothing = vec4(albedoColor, 1.0);
}