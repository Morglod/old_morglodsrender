#include "Material.hpp"

void MR::MaterialPass::Use(){
    if(diffuseTexture) MR::RenderManager::Instance()->UseTexture(diffuseTexture, diffuseTextureStage);
    if(shader) shader->Use();
}
