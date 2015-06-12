#version 400#extension GL_ARB_separate_shader_objects : enable
#extension GL_NV_shader_buffer_load : enable#pragma optimize (on)

/** BUILD CONFIG **/
#define MR_CUSTOM_INSTANCE_DATA 0
#define MR_UNIFORM_INSTANCE_DATA 0
#define MR_USE_ATTRIBUTE_TEXCOORD 1
#define MR_USE_ATTRIBUTE_NORMAL 1
#define MR_USE_ATTRIBUTE_COLOR 1
#define MR_USE_CUSTOM_VERTEX_PROCESSOR 0

/** VERTEX **/layout (location = 0) in vec3 MR_VERTEX_POSITION_ATTRIBUTE;
#if MR_USE_ATTRIBUTE_TEXCOORD == 1layout (location = 1) in vec2 MR_VERTEX_TEXCOORD_ATTRIBUTE;
#endif
#if MR_USE_ATTRIBUTE_NORMAL == 1layout (location = 2) in vec3 MR_VERTEX_NORMAL_ATTRIBUTE;
#endif
#if MR_USE_ATTRIBUTE_COLOR == 1layout (location = 3) in vec4 MR_VERTEX_COLOR_ATTRIBUTE;
#endif
/** PER ENTITY **/uniform mat4 MR_MAT_MODEL;

/** CAMERA INFO **/uniform mat4 MR_MAT_VIEW;uniform mat4 MR_MAT_PROJ;

/** INSTANCING **/#if MR_CUSTOM_INSTANCE_DATA == 0
struct MR_InstanceData {
    float pos_x;
    float pos_y;
    float pos_z;
};
#endif

#if MR_UNIFORM_INSTANCE_DATA == 1
uniform MR_InstanceData* MR_VERTEX_INSTANCED_POSITION;
#elselayout (location = 4) in MR_InstanceData* MR_VERTEX_INSTANCED_POSITION;
#endif

/** OUTPUT DATA **/
out vec3 MR_VertexPos;out vec3 MR_LocalVertexPos;
#if MR_USE_ATTRIBUTE_NORMAL == 1smooth out vec3 MR_VertexNormal;
#endifout vec4 MR_VertexColor;
#if MR_USE_ATTRIBUTE_TEXCOORD == 1out vec2 MR_VertexTexCoord;
#endifout vec3 MR_VertexInstancedPos;

/** BUILT-IN FUNCTIONS **/

#define MR_GetInstanceData() MR_VERTEX_INSTANCED_POSITION[gl_InstanceID];
#define MR_CalcMVP() (MR_MAT_PROJ * MR_MAT_VIEW * MR_MAT_MODEL)


/** MAIN **/
void main() {
    MR_InstanceData instanceData = MR_GetInstanceData();
    vec3 instancePos = vec3(instanceData.pos_x, instanceData.pos_y, instanceData.pos_z);
    MR_VertexInstancedPos = instancePos;

#if MR_USE_CUSTOM_VERTEX_PROCESSOR == 1
    /* CUSTOM VERTEX PROCESSOR CODE */
#else
    /** DEFAULT VERTEX PROCESSOR **/
    vec4 realPos = vec4(MR_VERTEX_POSITION_ATTRIBUTE + instancePos, 1.0);
    vec4 screenPos = MR_CalcMVP() * realPos;
    gl_Position = screenPos;
    MR_VertexPos = screenPos.xyz;    MR_LocalVertexPos = MR_VERTEX_POSITION_ATTRIBUTE;

#if MR_USE_ATTRIBUTE_NORMAL == 1
    MR_VertexNormal = (MR_MAT_MODEL * vec4(MR_VERTEX_NORMAL_ATTRIBUTE, 0.0)).xyz;
#endif

#if MR_USE_ATTRIBUTE_COLOR == 1
    MR_VertexColor = MR_VERTEX_COLOR_ATTRIBUTE;
#endif

#if MR_USE_ATTRIBUTE_TEXCOORD == 1
    MR_VertexTexCoord = MR_VERTEX_TEXCOORD_ATTRIBUTE;
#endif

#endif //END VERTEX PROCESSOR
}
