#pragma once

#include "mr/shader/program.hpp"
#include "mr/pre/glew.hpp"
#include "mr/pre/glm.hpp"
#include "mr/string.hpp"
#include "mr/shader/ubo.hpp"

#include <unordered_map>

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;
typedef std::shared_ptr<class Material> MaterialPtr;

class MR_API Material final {
public:
    void SetUBO(std::string const& name, UniformBufferPtr const& buf);
    inline ShaderProgramPtr GetProgram() const;

    UniformBufferPtr GetSystemUniformBuffer();
    static MaterialShaderPtr Create(ShaderProgramPtr const& program, std::vector<std::string> const& uniformNames);

protected:
    Material() = default;
    ShaderProgramPtr _program;
    std::unordered_map<std::string, UniformRefAny> _uniforms; // <name, ref>
};

inline ShaderProgramPtr Material::GetProgram() const {
    return _program;
}

}
