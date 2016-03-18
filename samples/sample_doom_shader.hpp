#pragma once

const char* vertexShader =
"#version 400 \n"
"#extension GL_ARB_bindless_texture : require \n"
"#extension GL_NV_gpu_shader5 : enable \n"
"#extension GL_NV_shader_buffer_load : enable \n"
"layout (location = 0) in vec3 pos; \n"
"layout (location = 1) in vec2 texCoord; \n"
"out vec4 gl_Position; \n"
"out vec2 vtexCoord; \n"
"struct InstanceData { \n"
"   float pos[3]; \n"
"}; \n"
"uniform UBOData { \n"
"   mat4 proj; \n"
"   mat4 view; \n"
"   mat4 model; \n"
"}; \n"
"uniform ModelData { \n"
"   uint64_t tex; \n"
"   InstanceData* instances; \n"
"}; \n"
"void main() { \n"
//"   gl_Position = ((proj * view * model) * vec4(pos + (objectPos[gl_InstanceID]), 1.0)); \n"
"   gl_Position = ((proj * view * model) * vec4(pos + vec3(instances[gl_InstanceID].pos[0],instances[gl_InstanceID].pos[1],instances[gl_InstanceID].pos[2]), 1.0)); \n"
"   vtexCoord = texCoord; \n"
"} \n"
;

const char* fragmentShader =
"#version 400 \n"
"#extension GL_ARB_bindless_texture : require \n"
"#extension GL_NV_gpu_shader5 : enable \n"
"#extension GL_NV_shader_buffer_load : enable \n"
"in vec2 vtexCoord; \n"
"out vec3 fragColor; \n"
"uniform float mr_time; \n"
"struct InstanceData { \n"
"   float pos[3]; \n"
"}; \n"
"uniform UBOData { \n"
"   mat4 proj; \n"
"   mat4 view; \n"
"   mat4 model; \n"
"}; \n"
"uniform ModelData { \n"
"   uint64_t tex; \n"
"   InstanceData* instances; \n"
"}; \n"
"void main() { \n"
"   fragColor = texture(sampler2D(tex), vtexCoord).rgb; \n"
"} \n"
;

struct InstanceData {
    float pos[3];
};

struct UBOData {
    glm::mat4 proj = glm::mat4(1.0f),
              view = glm::mat4(1.0f),
              model = glm::mat4(1.0f);
};

struct ModelData {
    uint64_t tex;
    uint64_t instances;
};

mr::BufferPtr ubo_data;
UBOData* ubo_data_ptr;

mr::ShaderProgramPtr shaderProgram;

mr::BufferPtr CreateModelData(uint64_t tex_resident, uint64_t instances_resident) {
    mr::Buffer::CreationFlags flags;
    return mr::Buffer::Create(mr::Memory::Own(MR_NEW_FILL(ModelData, {tex_resident, instances_resident}), sizeof(ModelData)), flags);
}

void InitShader() {
    using namespace mr;

    Buffer::CreationFlags flags; flags.map_after_creation = true;

    ubo_data = Buffer::Create(Memory::Own(MR_NEW(UBOData), sizeof(UBOData)), flags);
    ubo_data_ptr = (UBOData*)ubo_data->GetMapState().mem;

    auto vshader = Shader::Create(ShaderType::Vertex, std::string(vertexShader));
    auto fshader = Shader::Create(ShaderType::Fragment, std::string(fragmentShader));
    shaderProgram = ShaderProgram::Create({vshader, fshader});
    shaderProgram->UniformBuffer("UBOData", ubo_data, 0);
}
