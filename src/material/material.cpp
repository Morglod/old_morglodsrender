#include "mr/material/material.hpp"
#include "mr/vformat.hpp"
#include "src/shader/shader_code.hpp"
#include "mr/buffer.hpp"
#include "mr/string.hpp"
#include "mr/gl/types.hpp"
#include "mr/log.hpp"
#include "mr/texture.hpp"
#include "src/mp.hpp"
#include "mr/shader/uniformcache.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

void Material::UpdateShaderUniforms() {
    MP_ScopeSample(Material::UpdateShaderUniforms);

    static Material* lastMaterial = nullptr;
    if(lastMaterial == this) return;
    lastMaterial = this;

    if(_textures_num == 0) return;
    UniformCache* uniformCache = UniformCache::Get();

    for(uint32_t i = 0; i < _textures_num; ++i) {
        uniformCache->Set<uint64_t, 1>(_textures[i].uniformName, _textures[i].residentHandle);
    }
}

MaterialPtr Material::Create(ShaderProgramPtr const& program, std::vector<std::string> const& uniformNames, std::unordered_map<std::string, Texture2DPtr> const& textures) {
    MP_ScopeSample(Material::Create);
    MaterialPtr ms = MaterialPtr(MR_NEW(Material));
    ms->_program = program;
    for(std::string const& uniformName : uniformNames) {
        ShaderProgram::FoundUniform foundUniform;
        if(!program->FindUniform(uniformName, foundUniform)) {
            MR_LOG_WARNING(Material::Create, "Failed find uniform \""+uniformName+"\"");
        }
        else ms->_uniforms.insert(std::make_pair(uniformName, UniformRefAny(foundUniform.GetPtr(), foundUniform.size)));
    }

    if(textures.size() > 0) {
        ms->_textures_num = textures.size();
        ms->_textures = MR_NEW_ARRAY(sTexture, textures.size());

        uint32_t i = 0;
        for(auto const& UniformTexPair : textures) {
            auto const& texPtr = UniformTexPair.second;

            sTexture texture;
            texture.uniformName = UniformTexPair.first;
            texture.residentHandle = texPtr->GetResidentHandle();
            texture.texture = texPtr;
            ms->_textures[i] = texture;

            ++i;
        }
    } else ms->_textures_num = 0;

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
