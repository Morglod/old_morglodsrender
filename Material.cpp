#include "Material.hpp"
#include "RenderContext.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

void MR::MaterialPass::Use(RenderContext* rc){
    if( (_flag.always) || (_parent->GetManager()->ActivedFlag() == _flag.flag) ){
        if(_albedo)     rc->BindTexture(_albedo,        _albedoStage);
        if(_emissive)   rc->BindTexture(_emissive,      _emissiveStage);
        if(_environment)rc->BindTexture(_environment,   _environmentStage);
        if(_shader)     rc->UseShader(_shader);

        if(_twoSided)   glDisable(GL_CULL_FACE);
        else            glEnable(GL_CULL_FACE);
    }
}

void MR::MaterialPass::SetShader(Shader* sh) {
    _shader = sh;
    if(sh != nullptr) {
        _albedo_sample =            _shader->CreateUniform(MR_MATERIAL_ALBEDO_TEX, &_albedoStage);
        _emissive_sample =          _shader->CreateUniform(MR_MATERIAL_EMISSIVE_TEX, &_emissiveStage);
        _environment_sample =       _shader->CreateUniform(MR_MATERIAL_ENVIRONMENT_TEX, &_environmentStage);
        _uniform_a_discard =        _shader->CreateUniform(MR_MATERIAL_ALPHA_DISCARD_F, &_alphaDiscard);
        _uniform_color =            _shader->CreateUniform(MR_MATERIAL_COLOR_V4, _color);
        _uniform_emissive_color =   _shader->CreateUniform(MR_MATERIAL_EMISSIVE_V4, _emissive_color);
    }
}

MR::MaterialPass::MaterialPass(Material* mat) :
    _parent(mat), _shader(nullptr), _twoSided(false),
    _alphaDiscard(MR_MATERIAL_ALPHA_DISCARD_F_DEFAULT),
    _albedo(TextureManager::Instance()->WhiteTexture()), _albedoStage(MR_MATERIAL_ALBEDO_TEX_DEFAULT), _albedo_sample(nullptr),
    _emissive(nullptr), _emissiveStage(MR_MATERIAL_EMISSIVE_TEX_DEFAULT), _emissive_sample(nullptr),
    _environment(TextureManager::Instance()->WhiteTexture()), _environmentStage(MR_MATERIAL_ENVIRONMENT_TEX_DEFAULT), _environment_sample(nullptr),
    _color(new glm::vec4(MR_MATERIAL_COLOR_V4_DEFAULT)), _emissive_color(new glm::vec4(MR_MATERIAL_EMISSIVE_V4_DEFAULT)) {
}

MR::MaterialPass::~MaterialPass(){
}

MR::Material::Material(MaterialManager* mgr, const std::string& Name) :
    name(Name), manager(mgr) {
}
