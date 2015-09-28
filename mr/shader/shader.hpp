#pragma once

#include "../build.hpp"

#include <memory>
#include <future>

namespace mr {

typedef std::shared_ptr<class Shader> ShaderPtr;

enum class ShaderType : uint32_t {
    Vertex = 0x8B31,
    Fragment = 0x8B30
};

class MR_API Shader final {
public:
    inline uint32_t GetId() const;
    static std::future<ShaderPtr> Create(ShaderType const& type, std::string const& code);
protected:
    Shader();
private:
    static bool _Create(Shader* shader, uint32_t const& type, std::string const& code);
    static bool _Compile(Shader* shader, std::string const& code);
    uint32_t _id;
};

inline uint32_t Shader::GetId() const {
    return _id;
}

}
