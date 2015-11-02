#pragma once

#include "../build.hpp"
#include "../pre/glm.hpp"

#include <memory>
#include <future>
#include <vector>

namespace mr {

typedef std::shared_ptr<class ShaderProgram> ShaderProgramPtr;
typedef std::shared_ptr<class Shader> ShaderPtr;
typedef std::shared_ptr<class Buffer> BufferPtr;

class MR_API ShaderProgram final {
    friend class Draw;
public:
    // shaders may be empty
    static ShaderProgramPtr Create(std::vector<ShaderPtr> const& shaders = std::vector<ShaderPtr>());
    bool Link(std::vector<ShaderPtr> const& shaders);

    bool UniformMat4(std::string const& name, glm::mat4& mat);
    bool UniformBuffer(std::string const& name, BufferPtr const& buffer, uint32_t buffer_binding);
    bool UniformFloat(std::string const& name, float value);

    void Use();

    inline uint32_t GetId() const;

    virtual ~ShaderProgram();

protected:
    ShaderProgram();
    uint32_t _id;
};

inline uint32_t ShaderProgram::GetId() const {
    return _id;
}

}
