#include "Material.hpp"
#include "RenderSystem.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

void MR::MaterialPass::Use(IRenderSystem* rs){
    if( (_flag.always) || (_parent->GetManager()->ActivedFlag() == _flag.flag) ){
        if(_ambient)    rs->BindTexture(_ambient, _ambientUnit);
        if(_diffuse)    rs->BindTexture(_diffuse, _diffuseUnit);
        if(_opacity)    rs->BindTexture(_opacity, _opacityUnit);
        if(_shader) { rs->UseShader(_shader); }

        if(_twoSided)   glDisable(GL_CULL_FACE);
        else            glEnable(GL_CULL_FACE);
    }
}

void MR::MaterialPass::UnUse(IRenderSystem* rs){
    if(_ambient) rs->UnBindTexture(_ambientUnit);
    if(_diffuse) rs->UnBindTexture(_diffuseUnit);
    if(_opacity) rs->UnBindTexture(_opacityUnit);
    rs->UseShader(nullptr);
}

void MR::MaterialPass::SetShader(IShader* sh) {
    _shader = sh;
    if(sh != nullptr) {
        _ambient_sampler = _shader->CreateUniform(MR_SHADER_AMBIENT_TEX, IShaderUniform::Types::Int, &_ambientUnit);
        _diffuse_sampler = _shader->CreateUniform(MR_SHADER_DIFFUSE_TEX, IShaderUniform::Types::Int, &_diffuseUnit);
        _opacity_sampler = _shader->CreateUniform(MR_SHADER_OPACITY_TEX, IShaderUniform::Types::Int, &_opacityUnit);
        _uniform_color = _shader->CreateUniform(MR_SHADER_COLOR_V4, IShaderUniform::Types::Vec4, _color);

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

MR::Material::Material(MaterialManager* mgr, const std::string& Name) :
    name(Name), manager(mgr) {
}
