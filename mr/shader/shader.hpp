#pragma once

#include "../build.hpp"

#include <memory>
#include <future>

namespace mr {

typedef std::shared_ptr<class Shader> ShaderPtr;

enum class ShaderType : uint32_t {
    None = 0,
    Vertex = 0x8B31,
    Fragment = 0x8B30
};

class MR_API Shader final {
public:
    inline uint32_t GetId() const;
    inline ShaderType GetType() const;
    static ShaderPtr Create(ShaderType const& type, std::string const& code = std::string());
    bool Compile(std::string const& code);
    virtual ~Shader();
protected:
    Shader();
private:
    uint32_t _id;
    ShaderType _type;
};

inline uint32_t Shader::GetId() const {
    return _id;
}

inline ShaderType Shader::GetType() const {
    return _type;
}

}
