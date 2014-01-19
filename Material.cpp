#include "Material.hpp"
#include "RenderContext.hpp"
#include "Texture.hpp"

void MR::MaterialPass::Use(RenderContext* rc){
    if(diffuseTexture) rc->UseTexture(diffuseTexture, diffuseTextureStage);
    if(shader) rc->UseShader(shader);
}
