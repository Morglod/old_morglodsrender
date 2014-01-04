#include "Material.hpp"

void MR::MaterialPass::Use(){
    if(ambientColor) glColor4f(ambientColor[0], ambientColor[1], ambientColor[2], ambientColor[3]);
    if(ambientTexture) ambientTexture->Bind(ambientTextureStage);
    if(shader) shader->Use();
}
