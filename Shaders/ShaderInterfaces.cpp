#include "ShaderInterfaces.hpp"

namespace mr {

const std::string __MR_SHADER_UNIFORM_TYPE_NAMES[] {
    std::string("float"),
    std::string("int"),
    std::string("mat4"),
    std::string("vec2"),
    std::string("vec3"),
    std::string("vec4"),
    std::string("uint64"),
    std::string("sampler1D"),
    std::string("sampler2D"),
    std::string("sampler3D")
};

std::string IShaderUniformRef::TypeToString(IShaderUniformRef::Type const& t) {
    return __MR_SHADER_UNIFORM_TYPE_NAMES[(int)t];
}

ShaderProgramCache& ShaderProgramCache::operator = (ShaderProgramCache& c) {
    if(this == &c) {
        return *this;
    }
    format = c.format;
    data = c.data;
    return *this;
}

ShaderProgramCache::ShaderProgramCache() : format(0), data() {}
ShaderProgramCache::ShaderProgramCache(unsigned int const& f, mu::ArrayHandle<unsigned char> d) : format(f), data(d) {}

}
