#version 400
#extension GL_NV_shader_buffer_load : enable

/** BUILD CONFIG **/

#define MR_CUSTOM_INSTANCE_DATA 0
#define MR_UNIFORM_INSTANCE_DATA 1
#define MR_USE_ATTRIBUTE_TEXCOORD 1
#define MR_USE_ATTRIBUTE_NORMAL 1
#define MR_USE_ATTRIBUTE_COLOR 1
#define MR_USE_CUSTOM_VERTEX_PROCESSOR 0

/** VERTEX **/

#if MR_USE_ATTRIBUTE_TEXCOORD == 1
#endif
#if MR_USE_ATTRIBUTE_NORMAL == 1
#endif
#if MR_USE_ATTRIBUTE_COLOR == 1
#endif
/** PER ENTITY **/


/** CAMERA INFO **/

uniform vec3 MR_CAM_POS;

/** INSTANCING **/

struct MR_InstanceData {
    float pos_x;
    float pos_y;
    float pos_z;
};
#endif

#if MR_UNIFORM_INSTANCE_DATA == 1
uniform MR_InstanceData* MR_VERTEX_INSTANCED_POSITION;
#else
#endif

/** OUTPUT DATA **/

out vec3 MR_VertexPos;
#if MR_USE_ATTRIBUTE_NORMAL == 1
#endif
#if MR_USE_ATTRIBUTE_TEXCOORD == 1
#endif

/** BUILT-IN FUNCTIONS **/

#define MR_GetInstanceData() MR_VERTEX_INSTANCED_POSITION[gl_InstanceID];
#define MR_CalcMVP() (MR_MAT_PROJ * MR_MAT_VIEW * MR_MAT_MODEL)


/** MAIN **/

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
    MR_VertexPos = screenPos.xyz;

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
