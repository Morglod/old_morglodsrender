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
    inline uint32_t GetId() const;
    std::future<bool> UniformMat4(std::string const& name, glm::mat4 const& mat);
    std::future<bool> UniformBuffer(std::string const& name, BufferPtr const& buffer, uint32_t buffer_binding);
    static std::future<ShaderProgramPtr> Create(std::vector<ShaderPtr> const& shaders);
    uint32_t _id;
protected:
    ShaderProgram();
private:
    static bool _Create(ShaderProgram* prog, std::vector<ShaderPtr> const& shaders);
    static bool _Link(ShaderProgram* prog, std::vector<ShaderPtr> const& shaders);
    static bool _UniformMat4(ShaderProgram* prog, std::string const& name, glm::mat4 const& mat);
    static bool _UniformBuffer(ShaderProgram* prog, std::string const& name, Buffer* buffer, uint32_t buffer_binding);
};

inline uint32_t ShaderProgram::GetId() const {
    return _id;
}

}
