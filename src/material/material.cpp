#include "mr/material/material.hpp"
#include "mr/vformat.hpp"
#include "src/shader/shader_code.hpp"
#include "mr/buffer.hpp"
#include "mr/string.hpp"
#include "mr/gl/types.hpp"
#include "mr/log.hpp"
#include "mr/texture.hpp"
#include "src/mp.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

void Material::UpdateShaderUniforms() {
    MP_ScopeSample(Material::UpdateShaderUniforms);

    static Material* lastMaterial = nullptr;
    if(lastMaterial == this) return;
    lastMaterial = this;

    glFinish(); // TODO, flush mapped buffer memory
    for(uint32_t i = 0; i < _textures_num; ++i) {
        //if(_textures[i].texture == nullptr) continue;
        _textures[i].uniform = _textures[i].residentHandle;
    }
}

MaterialPtr Material::Create(ShaderProgramPtr const& program, std::vector<std::string> const& uniformNames, std::unordered_map<std::string, Texture2DPtr> const& textures) {
    MP_ScopeSample(Material::Create);
    MaterialPtr ms = MaterialPtr(new Material);
    ms->_program = program;
    for(std::string const& uniformName : uniformNames) {
        ShaderProgram::FoundUniform foundUniform;
        if(!program->FindUniform(uniformName, foundUniform)) {
            MR_LOG_WARNING(Material::Create, "Failed find uniform \""+uniformName+"\"");
        }
        else ms->_uniforms.insert(std::make_pair(uniformName, UniformRefAny(foundUniform.GetPtr(), foundUniform.size)));
    }
    ms->_textures = new sTexture[textures.size()];
    uint32_t i = 0;
    for(auto const& textureInfo : textures) {
        ShaderProgram::FoundUniform foundUniform;
        if(!program->FindUniform(textureInfo.first, foundUniform)) {
            MR_LOG_WARNING(Material::Create, "Failed find uniform \""+textureInfo.first+"\"");
        }
        else {
            sTexture texture;
            texture.residentHandle = textureInfo.second->GetResidentHandle();
            texture.uniform = UniformRefAny(foundUniform.GetPtr(), foundUniform.size);
            texture.texture = textureInfo.second;
            texture.uniformName = foundUniform.uniformName;
            ms->_textures[i] = texture;
        }
        ++i;
    }
    return ms;
}

Material::~Material() {
    if(_textures != nullptr) {
        delete [] _textures;
        _textures = nullptr;
        _textures_num = 0;
    }
}

}
