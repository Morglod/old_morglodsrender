#include "ShaderInterfaces.hpp"

namespace mr {

const std::string __MR_SHADER_UNIFORM_TYPE_NAMES[] {
    std::string("float"),
    std::string("int"),
    std::string("mat4"),
    std::string("vec2"),
    std::string("vec3"),
    std::string("vec4"),
    std::string("sampler1D"),
    std::string("sampler2D"),
    std::string("sampler3D")
};

std::string IShaderUniform::TypeToString(const IShaderUniform::Type& t) {
    return __MR_SHADER_UNIFORM_TYPE_NAMES[(int)t];
}

ShaderUniformInfo::ShaderUniformInfo() : program(nullptr), name("NoName"), uniform_size(0), uniform_gl_type(0) {
}

ShaderUniformInfo::ShaderUniformInfo(IShaderProgram* p, const std::string& n, const int& s, const unsigned int & t)
: program(p), name(n), uniform_size(s), uniform_gl_type(t) {}

ShaderProgramCache& ShaderProgramCache::operator = (ShaderProgramCache& c) {
    if(this == &c) {
        return *this;
    }
    format = c.format;
    data = c.data;
    return *this;
}

ShaderProgramCache::ShaderProgramCache() : format(0), data() {}
ShaderProgramCache::ShaderProgramCache(unsigned int const& f, mr::TStaticArray<unsigned char> d) : format(f), data(d) {}

}
