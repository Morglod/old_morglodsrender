#include "Material.hpp"
#include "../Utils/Log.hpp"
#include "../RenderManager.hpp"
#include "../Shaders/ShaderInterfaces.hpp"
#include "MaterialsConfig.hpp"
#include "../Utils/Debug.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

namespace MR {

bool MaterialPass::Use() {
    MR::RenderManager* rm = MR::RenderManager::GetInstance();
    if( (_flag.always) || (rm->GetActivedMaterialFlag() == _flag.flag) ){

        if(rm->GetActivedMaterialPass() == this) {
            _program->UpdateUniforms();
            return true;
        }

        bool bStatus = _program->Use();

        for(size_t i = 0; i < _textures.GetNum(); ++i){
            switch(_textures.KeyAt(i)) {
            case TT_Albedo:
                rm->SetTexture(_textures.ValueAt(i), MR_MATERIAL_ALBEDO_TEX_DEFAULT);
                break;
            case TT_Emission:
                rm->SetTexture(_textures.ValueAt(i), MR_MATERIAL_EMISSION_TEX_DEFAULT);
                break;
            case TT_OpacityMask:
                rm->SetTexture(_textures.ValueAt(i), MR_MATERIAL_OPACITYMASK_TEX_DEFAULT);
                break;
            }
        }

        if(_two_sided)   glDisable(GL_CULL_FACE);
        else            glEnable(GL_CULL_FACE);

        rm->SetActivedMaterialPass(dynamic_cast<MR::IMaterialPass*>(this));

        return bStatus;
    } else {
        return false;
    }
}

void MaterialPass::SetTexture(const TextureType& type, ITexture* tex) {
    _textures[type] = tex;
}

ITexture* MaterialPass::GetTexture(const TextureType& type) {
    return _textures[type];
}

MaterialPass::MaterialPass() : _flag(), _program(nullptr), _textures(), _two_sided(false), _color(1,1,1,1) {
}

MaterialPass::MaterialPass(const MaterialFlag& f, IShaderProgram* sp, const bool& twoSided, const glm::vec4& c)  : _flag(f), _program(sp), _textures(), _two_sided(twoSided), _color(c) {
}

MaterialPass::~MaterialPass() {
}

// MATERIAL

MR::TStaticArray<IMaterialPass*> Material::GetPasses(const MaterialFlag& flag) {
    std::vector<IMaterialPass*> passes;
    for(size_t i = 0; i < _passes.size(); ++i){
        if(_passes[i]->GetFlag() == flag) {
            passes.push_back(_passes[i]);
        }
    }
    return MR::TStaticArray<IMaterialPass*>(passes.data(), passes.size(), true);
}

IMaterialPass* Material::CreatePass(const MaterialFlag& flag) {
    MaterialPass* mp = new MaterialPass();
    mp->SetFlag(flag);
    AddPass(mp);
    return dynamic_cast<IMaterialPass*>(mp);
}

void Material::RemovePass(IMaterialPass* pass) {
    auto it = std::find(_passes.begin(), _passes.end(), pass);
    if(it == _passes.end()) return;
    _passes.erase(it);
}

void Material::RemovePass(const size_t& index) {
    _passes.erase(_passes.begin()+index);
}

void Material::SetShaderProgram(const MaterialFlag& flag, IShaderProgram* sp) {
    for(size_t i = 0; i < _passes.size(); ++i){
        if(_passes[i]->GetFlag() == flag) {
            _passes[i]->SetShaderProgram(sp);
        }
    }
}

void Material::SetTexture(const MaterialFlag& flag, const IMaterialPass::TextureType& type, ITexture* tex) {
    for(size_t i = 0; i < _passes.size(); ++i){
        if(_passes[i]->GetFlag() == flag) {
            _passes[i]->SetTexture(type, tex);
        }
    }
}

void Material::SetTwoSided(const MaterialFlag& flag, const bool& s) {
    for(size_t i = 0; i < _passes.size(); ++i){
        if(_passes[i]->GetFlag() == flag) {
            _passes[i]->SetTwoSided(s);
        }
    }
}

void Material::SetColor(const MaterialFlag& flag, const glm::vec4& color) {
    for(size_t i = 0; i < _passes.size(); ++i){
        if(_passes[i]->GetFlag() == flag) {
            _passes[i]->SetColor(color);
        }
    }
}

Material::Material() : _passes() {
}

Material::Material(MR::TStaticArray<IMaterialPass*> passes) : _passes() {
    for(size_t i = 0; i < passes.GetNum(); ++i){
        _passes.push_back(passes.At(i));
    }
}

Material::~Material() {
    _passes.clear();
}

}
