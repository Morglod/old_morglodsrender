#pragma once

#include "mr/shader/program.hpp"
#include "mr/pre/glew.hpp"
#include "mr/pre/glm.hpp"
#include "mr/string.hpp"
#include "mr/shader/ubo.hpp"

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;
typedef std::shared_ptr<class MaterialShader> MaterialShaderPtr;

class MR_API MaterialShader final {
public:
    void SetUBO(std::string const& name, UniformBufferPtr const& buf);
    inline ShaderProgramPtr GetProgram() const;

    UniformBufferPtr GetSystemUniformBuffer();
    static MaterialShaderPtr Create(ShaderProgramPtr const& program, bool createBuffers = true);
protected:
    bool _Init(bool createBuffers);
    MaterialShader() = default;
    ShaderProgramPtr _program;
    struct sUBO {
        std::string name;
        UniformBufferPtr buffer = nullptr;
    };
    std::vector<sUBO> _ubos;
};

inline ShaderProgramPtr MaterialShader::GetProgram() const {
    return _program;
}

}
