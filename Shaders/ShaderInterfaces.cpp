#include "ShaderInterfaces.hpp"

namespace MR {

const std::string __MR_SHADER_UNIFORM_TYPE_NAMES[] {
    "float",
    "int",
    "mat4",
    "vec2",
    "vec3",
    "vec4",
    "sampler1D",
    "sampler2D",
    "sampler3D"
};

std::string IShaderUniform::TypeToString(const IShaderUniform::Type& t) {
    return __MR_SHADER_UNIFORM_TYPE_NAMES[(int)t];
}

ShaderUniformInfo::ShaderUniformInfo(IShaderProgram* p, const std::string& n, const int& s, const unsigned int & t)
: program(p), name(n), uniform_size(s), uniform_gl_type(t) {}

bool MR::IShaderProgram::Features::operator==(const MR::IShaderProgram::Features& dsr1) const {
    return (
               (dsr1.colorFilter == colorFilter) &&
               (dsr1.opacityDiscardOnAlpha == opacityDiscardOnAlpha) &&
               (dsr1.opacityDiscardValue == opacityDiscardValue) &&
               (dsr1.ambient == ambient) &&
               (dsr1.diffuse == diffuse) &&
               (dsr1.displacement == displacement) &&
               (dsr1.emissive == emissive) &&
               (dsr1.height == height) &&
               (dsr1.baked_lightmap == baked_lightmap) &&
               (dsr1.normal == normal) &&
               (dsr1.opacity == opacity) &&
               (dsr1.reflection == reflection) &&
               (dsr1.shininess == shininess) &&
               (dsr1.specular == specular) &&
               (dsr1.toRenderTarget == toRenderTarget) &&
               (dsr1.toScreen == toScreen) &&
               (dsr1.light == light) &&
               (dsr1.fog == fog)   );
}

}
