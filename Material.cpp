#include "Material.hpp"
#include "Shaders/ShaderInterfaces.hpp"
#include "Utils/Singleton.hpp"
#include "RenderManager.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

bool MR::MaterialPass::Use(){
    if( (_flag.always) || (_parent->GetManager()->GetActivedFlag() == _flag.flag) ){
        MR::RenderManager * rm = MR::RenderManager::GetInstance();
        rm->UseShaderProgram(_shader);

        if(_ambient)    rm->BindTexture(_ambient, _ambientUnit);
        if(_diffuse)    rm->BindTexture(_diffuse, _diffuseUnit);
        if(_opacity)    rm->BindTexture(_opacity, _opacityUnit);

        if(_twoSided)   glDisable(GL_CULL_FACE);
        else            glEnable(GL_CULL_FACE);

        return true;
    } else return false;
}

void MR::MaterialPass::UnUse(){
    MR::RenderManager * rm = MR::RenderManager::GetInstance();
    if(_ambient) rm->UnBindTexture(_ambientUnit);
    if(_diffuse) rm->UnBindTexture(_diffuseUnit);
    if(_opacity) rm->UnBindTexture(_opacityUnit);
    rm->UseShaderProgram(nullptr);
}

void MR::MaterialPass::SetShader(IShaderProgram* sh) {
    _shader = sh;
    if(sh != nullptr) {
        _ambient_sampler = _shader->CreateUniform(MR_SHADER_AMBIENT_TEX, IShaderUniform::SUT_Int, &_ambientUnit);
        _diffuse_sampler = _shader->CreateUniform(MR_SHADER_DIFFUSE_TEX, IShaderUniform::SUT_Int, &_diffuseUnit);
        _opacity_sampler = _shader->CreateUniform(MR_SHADER_OPACITY_TEX, IShaderUniform::SUT_Int, &_opacityUnit);
        _uniform_color = _shader->CreateUniform(MR_SHADER_COLOR_V4, IShaderUniform::SUT_Vec4, _color);

        /*_emissive_sampler =          _shader->CreateUniform(MR_SHADER_EMISSIVE_TEX, &_emissiveStage);
        _environment_sampler =       _shader->CreateUniform(MR_SHADER_ENVIRONMENT_TEX, &_environmentStage);
        _uniform_a_discard =        _shader->CreateUniform(MR_SHADER_ALPHA_DISCARD_F, &_alphaDiscard);
        _uniform_color =            _shader->CreateUniform(MR_SHADER_COLOR_V4, _color);
        _uniform_emissive_color =   _shader->CreateUniform(MR_SHADER_EMISSIVE_V4, _emissive_color);*/
    }
}

MR::MaterialPass::MaterialPass(Material* mat) :
    _parent(mat), _shader(nullptr), _twoSided(false),
    _ambient(nullptr),
    _ambientUnit(MR_MATERIAL_AMBIENT_TEX_DEFAULT),
    _ambient_sampler(nullptr),
    _diffuse(nullptr),
    _diffuseUnit(MR_MATERIAL_DIFFUSE_TEX_DEFAULT),
    _diffuse_sampler(nullptr),
    _opacity(nullptr),
    _opacityUnit(MR_MATERIAL_OPACITY_TEX_DEFAULT),
    _opacity_sampler(nullptr),
    _color(new glm::vec4(MR_MATERIAL_COLOR_V4_DEFAULT))
{
}

MR::MaterialPass::~MaterialPass(){
}

void MR::Material::SetShader(IShaderProgram* sh) {
    for(size_t i = 0; i < materialPasses.size(); ++i){
        materialPasses[i]->SetShader(sh);
    }
}

void MR::Material::SetAmbientTexture(ITexture* t) {
    for(size_t i = 0; i < materialPasses.size(); ++i){
        materialPasses[i]->SetAmbientTexture(t);
    }
}

void MR::Material::SetAmbientTextureUnit(const unsigned int& s) {
    for(size_t i = 0; i < materialPasses.size(); ++i){
        materialPasses[i]->SetAmbientTextureUnit(s);
    }
}

void MR::Material::SetDiffuseTexture(ITexture* t) {
    for(size_t i = 0; i < materialPasses.size(); ++i){
        materialPasses[i]->SetDiffuseTexture(t);
    }
}

void MR::Material::SetDiffuseTextureUnit(const unsigned int& s) {
    for(size_t i = 0; i < materialPasses.size(); ++i){
        materialPasses[i]->SetDiffuseTextureUnit(s);
    }
}

void MR::Material::SetOpacityTexture(ITexture* t) {
    for(size_t i = 0; i < materialPasses.size(); ++i){
        materialPasses[i]->SetOpacityTexture(t);
    }
}

void MR::Material::SetOpacityTextureUnit(const unsigned int& s) {
    for(size_t i = 0; i < materialPasses.size(); ++i){
        materialPasses[i]->SetOpacityTextureUnit(s);
    }
}

void MR::Material::SetTwoSided(const bool& ts) {
    for(size_t i = 0; i < materialPasses.size(); ++i){
        materialPasses[i]->SetTwoSided(ts);
    }
}

MR::Material::Material(MaterialManager* mgr, const std::string& Name) :
    name(Name), manager(mgr) {
}
