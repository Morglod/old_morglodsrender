#pragma once

#include "mr/shader/program.hpp"
#include "mr/pre/glew.hpp"
#include "mr/pre/glm.hpp"
#include "mr/string.hpp"

#define _MR_TO_STRING(x) (#x)

#define _MR_DEF_SYS_UNIFORM(x) \
struct SysUniformData { \
    x \
}; \
const std::string SysUniformStr = "uniform "+SysUniformNameBlock+" { " +ReplaceString((#x), "glm::", "")+ " } "+SysUniformName+"; \n";

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;
typedef std::shared_ptr<class MaterialShader> MaterialShaderPtr;

/// const std::string SysUniformStr;
const std::string SysUniformName = "mr_sys";
const std::string SysUniformNameBlock = SysUniformName+"_block";

_MR_DEF_SYS_UNIFORM(
	glm::mat4 view;
	glm::mat4 proj;
	float time;
)

class MR_API MaterialShader final {
public:
    void SetUBO(std::string const& name, BufferPtr const& buf);
    void SetUBO(int32_t index, BufferPtr const& buf);
    BufferPtr GetUBO(std::string const& name);
    BufferPtr GetUBO(int32_t index);
    std::string GetUBOName(int32_t index);
    int32_t GetUBOSize(int32_t index);
    inline int32_t GetUBONum() const;
    inline ShaderProgramPtr GetProgram() const;

    static BufferPtr GetSystemUniformBuffer();
    static MaterialShaderPtr Create(ShaderProgramPtr const& program, bool createBuffers = true);
protected:
    bool _Init(bool createBuffers);
    MaterialShader() = default;
    ShaderProgramPtr _program;
    struct sUBO {
        std::string name;
        BufferPtr buffer;
    };
    std::vector<sUBO> _ubos;
};

inline int32_t MaterialShader::GetUBONum() const {
    return _ubos.size();
}

inline ShaderProgramPtr MaterialShader::GetProgram() const {
    return _program;
}

}

#undef _MR_DEF_SYS_UNIFORM
