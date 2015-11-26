#pragma once

#include "mr/build.hpp"
#include "mr/pre/glm.hpp"

#include <memory>
#include <future>
#include <vector>

namespace mr {

typedef std::shared_ptr<class ShaderProgram> ShaderProgramPtr;
typedef std::shared_ptr<class Shader> ShaderPtr;
typedef std::shared_ptr<class UniformBuffer> UniformBufferPtr;
typedef std::shared_ptr<class UniformBufferDecl> UniformBufferDeclPtr;

class MR_API ShaderProgram final {
    friend class Draw;
public:
    struct UBO {
        std::string name = "";
        UniformBufferPtr ubo = nullptr;
    };

    // shaders may be empty
    static ShaderProgramPtr Create(std::vector<ShaderPtr> const& shaders = std::vector<ShaderPtr>());
    bool Link(std::vector<ShaderPtr> const& shaders);

    bool SetUniformBuffer(uint32_t arrayIndex, UniformBufferPtr const& ubo);
    bool SetUniformBuffer(std::string const& name, UniformBufferPtr const& ubo);
    inline uint32_t GetUniformBuffersNum() const;
    bool GetUniformBuffer(std::string const& name, ShaderProgram::UBO& out_ubo);
    bool GetUniformBuffer(uint32_t arrayIndex, ShaderProgram::UBO& out_ubo) const;

    static void Use(ShaderProgramPtr const& program);

    inline uint32_t GetId() const;

    virtual ~ShaderProgram();

protected:
    void _BindUniformBuffer(uint32_t index, uint32_t buffer);

    ShaderProgram();
    bool _InitUBO();
    uint32_t _id;

    struct sUBOList {
        UBO* arr = nullptr;
        uint32_t num = 0;

        void Resize(uint32_t num);
        void Free();
        ~sUBOList();
    };

    sUBOList _ubo;
};

///

inline uint32_t ShaderProgram::GetId() const {
    return _id;
}

inline uint32_t ShaderProgram::GetUniformBuffersNum() const {
    return _ubo.num;
}

}
