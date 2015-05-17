#version 330#extension GL_ARB_separate_shader_objects : enable
#extension GL_NV_shader_buffer_load : enable
#pragma optimize (on)
struct Vec3Ptr {
    float x;
    float y;
    float z;
};
layout (location = 0) in vec3 MR_VERTEX_POSITION_ATTRIBUTE;layout (location = 1) in vec2 MR_VERTEX_TEXCOORD_ATTRIBUTE;layout (location = 2) in vec3 MR_VERTEX_NORMAL_ATTRIBUTE;layout (location = 3) in vec4 MR_VERTEX_COLOR_ATTRIBUTE;layout (location = 4) in Vec3Ptr* MR_VERTEX_INSTANCED_POSITION_ATTRIBUTE;uniform mat4 MR_MAT_MVP;uniform mat4 MR_MAT_MODEL;uniform mat4 MR_MAT_VIEW;uniform mat4 MR_MAT_PROJ;out vec4 MR_VertexPos;out vec4 MR_LocalVertexPos;smooth out vec3 MR_VertexNormal;out vec4 MR_VertexColor;out vec2 MR_VertexTexCoord;out vec3 MR_VertexInstancedPos;void main() {
    vec3 inst_pos = vec3(MR_VERTEX_INSTANCED_POSITION_ATTRIBUTE[gl_InstanceID].x, MR_VERTEX_INSTANCED_POSITION_ATTRIBUTE[gl_InstanceID].y, MR_VERTEX_INSTANCED_POSITION_ATTRIBUTE[gl_InstanceID].z);	vec4 pos = MR_MAT_MVP * MR_MAT_MODEL * vec4(MR_VERTEX_POSITION_ATTRIBUTE + inst_pos, 1);	MR_VertexPos = pos;    MR_LocalVertexPos = vec4(MR_VERTEX_POSITION_ATTRIBUTE,1);    MR_VertexNormal = MR_VERTEX_NORMAL_ATTRIBUTE;	MR_VertexColor = MR_VERTEX_COLOR_ATTRIBUTE;	MR_VertexTexCoord = MR_VERTEX_TEXCOORD_ATTRIBUTE;
	MR_VertexInstancedPos = inst_pos;	gl_Position = pos;}
