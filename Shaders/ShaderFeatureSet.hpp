#pragma once

#include "../Textures/TextureInterfaces.hpp"

#include <string>

namespace mr {

class ShaderFeatureSet {
public:
    bool bindlessTextures = false;

    /*
        Pack all uniforms as uniform blocks and store in buffers.
        Not supported
    */
    //bool uniformBlocks = false;

    std::string ShaderHeader();
    std::string TextureHandleUniform(std::string uniformName, mr::ITexture::Types const& textureType);

    /*
        Bind texture to unit if !bindlessTextures
    */
    bool SetUniformTexture(ITexture* tex, class IShaderUniform* shaderUniform);
};

}
