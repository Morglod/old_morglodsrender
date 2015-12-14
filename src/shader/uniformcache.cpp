#include "mr/shader/uniformcache.hpp"
#include "mr/shader/program.hpp"
#include "mr/pre/glew.hpp"
#include "mr/pre/glm.hpp"
#include "mr/log.hpp"

namespace {

mr::UniformCache _uniformCache;

}

namespace mr {

template<>
MR_API void IUniformCacheValue::Update<float>(const ShaderProgram* shaderProgram, std::string const& name, int32_t num, const float* data) {
    glProgramUniform1fv(shaderProgram->GetId(), shaderProgram->GetUniformLocation(name), num, data);
}

template<>
MR_API void IUniformCacheValue::Update<int>(const ShaderProgram* shaderProgram, std::string const& name, int32_t num, const int* data) {
    glProgramUniform1iv(shaderProgram->GetId(), shaderProgram->GetUniformLocation(name), num, data);
}

template<>
MR_API void IUniformCacheValue::Update<double>(const ShaderProgram* shaderProgram, std::string const& name, int32_t num, const double* data) {
    glProgramUniform1dv(shaderProgram->GetId(), shaderProgram->GetUniformLocation(name), num, data);
}

template<>
MR_API void IUniformCacheValue::Update<uint64_t>(const ShaderProgram* shaderProgram, std::string const& name, int32_t num, const uint64_t* data) {
    if(GLEW_ARB_gpu_shader_int64) glProgramUniform1ui64vARB(shaderProgram->GetId(), shaderProgram->GetUniformLocation(name), num, data);
    else if(GLEW_NV_gpu_shader5) glProgramUniform1ui64vNV(shaderProgram->GetId(), shaderProgram->GetUniformLocation(name), num, data);
    else {
        MR_LOG_ERROR(IUniformCacheValue::Update<uint64_t>, "nor GLEW_ARB_gpu_shader_int64, GLEW_NV_gpu_shader5 is supported");
    }
}

template<>
MR_API void IUniformCacheValue::Update<glm::mat4>(const ShaderProgram* shaderProgram, std::string const& name, int32_t num, const glm::mat4* data) {
    glProgramUniformMatrix4fv(shaderProgram->GetId(), shaderProgram->GetUniformLocation(name), num, false, &data[0][0][0]);
}

UniformCache* UniformCache::Get() {
    return &_uniformCache;
}

void UniformCache::_Update(IUniformCacheValue* value) {
    const auto programs = ShaderProgram::GetRegisteredPrograms();
    for(const ShaderProgram* const program : programs) {
        value->Update(program);
    }
}

}
