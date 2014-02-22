#include "Material.hpp"
#include "RenderContext.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

void MR::MaterialPass::Use(RenderContext* rc){
    if( (flag.always) || (parent->GetManager()->ActivedFlag() == flag.flag) ){
        if(diffuseTexture) rc->BindTexture(diffuseTexture, diffuseTextureStage);
        if(shader) rc->UseShader(shader);
        if(twoSided) glDisable(GL_CULL_FACE);
        else glEnable(GL_CULL_FACE);
    }
}

void MR::MaterialPass::SetShader(Shader* sh) {
    shader = sh;
    if(sh != nullptr) {
        _u_ambientLight = shader->CreateUniform("ENGINE_PASS_AMBIENT", &_ambientLight);
    }
}

MR::MaterialPass::MaterialPass(Material* mat) :
    parent(mat),
    diffuseTexture(nullptr), diffuseTextureStage(GL_TEXTURE0),
    shader(nullptr), twoSided(false), alphaDiscard(false), _ambientLight(1.0f, 1.0f, 1.0f, 1.0f) {
}

MR::MaterialPass::MaterialPass(Material* mat, Texture* dTex, GLenum dTexStage, Shader* sh) :
    parent(mat),
    diffuseTexture(dTex), diffuseTextureStage(dTexStage), twoSided(false), alphaDiscard(false), _ambientLight(1.0f, 1.0f, 1.0f, 1.0f) {

    SetShader(sh);
}

MR::Material::Material(MaterialManager* mgr, const std::string& Name) :
    name(Name), manager(mgr) {
}
