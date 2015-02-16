#include "ShaderFeatureSet.hpp"

#include "ShaderObjects.hpp"
#include "../MachineInfo.hpp"

namespace mr {

std::string ShaderFeatureSet::ShaderHeader() {
    static bool gl43 = mr::gl::GetVersionAsFloat() >= 4.3f;
    std::string header = "";

    if(bindlessTextures) {
        header += "#extension GL_ARB_bindless_texture : require\n";
    }

    return header;
}

std::string ShaderFeatureSet::TextureHandleUniform(std::string uniformName, mr::ITexture::Types const& textureType) {
    static bool gl43 = mr::gl::GetVersionAsFloat() >= 4.3f;

    //There are spaces in type names
    const std::string samplerTypes[] = {
        "uniform sampler1D ",
        "uniform sampler2D ",
        "uniform sampler3D "
    };

    if(bindlessTextures) {
        if(gl43) {
            //glUniformHandle* function to bind
            return "layout (bindless_sampler) " + samplerTypes[(size_t)textureType] + uniformName;
        }
        #warning May be "uvec2" type?
        return samplerTypes[(size_t)textureType] + uniformName;
    } else {
        if(gl43) {
            return "layout (bound_sampler) " + samplerTypes[(size_t)textureType] + uniformName;
        }
        return samplerTypes[(size_t)textureType] + uniformName;
    }
}

bool ShaderFeatureSet::SetUniformTexture(ITexture* tex, IShaderUniform* shaderUniform) {
    IShaderProgram* was = nullptr;
    IShaderProgram* prog = shaderUniform->GetParent();

    if(!mr::gl::IsDirectStateAccessSupported()) {
        was = GetUsedShaderProgram();
        if(was != prog) {
            if(!prog->Use()) {
                if(was != nullptr) was->Use();
                return false;
            }
        } else was = nullptr;
    }

    if(bindlessTextures) {
        if(mr::gl::IsDirectStateAccessSupported()) glProgramUniformHandleui64ARB(prog->GetGPUHandle(), shaderUniform->GetGPULocation(), value);
        else glUniformHandleui64ARB(uniformLocation, shaderUniform->GetGPULocation(), value);
    } else {
        if(mr::gl::IsDirectStateAccessSupported()) glProgramUniform1i(prog->GetGPUHandle(), shaderUniform->GetGPULocation(), tex->GetLastBindedUnit());
        else glUniform1i(shaderUniform->GetGPULocation(), tex->GetLastBindedUnit());
    }

    if(was != nullptr) was->Use();
    return true;
}

}
