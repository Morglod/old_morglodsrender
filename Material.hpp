#pragma once

#ifndef _MR_MATERIAL_H_
#define _MR_MATERIAL_H_

#include "Config.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

#include <string>
#include <vector>

namespace MR {

class RenderContext;

class Shader;
class ShaderUniform;
class IShaderUniform;

class ITexture;

class IRenderSystem;

class Material;
class MaterialManager;
struct MaterialFlag;

struct MaterialFlag {
public:
    bool always; //use material pass always (flag wouldn't change anything)
    unsigned char flag; //if always==false, use this pass only if this flag is On
    MaterialFlag() : always(false), flag(0) {}
    MaterialFlag(bool Always, unsigned char Flag) : always(Always), flag(Flag) {}

    inline static MaterialFlag Default(){return MaterialFlag(false, 0);} //default rendering state
    inline static MaterialFlag ShadowMap(){return MaterialFlag(false, 1);} //drawing to shadow map
};

class MaterialPass {
public:
    virtual void Use(IRenderSystem* rs);
    inline MaterialFlag GetFlag();
    inline MaterialFlag* GetFlagPtr();

    inline Material* GetMaterial();
    inline Shader* GetShader();
    virtual void SetShader(Shader* sh);

    inline ITexture* GetAlbedoTexture();
    inline unsigned int GetAlbedoTextureStage();

    inline void SetAlbedoTexture(ITexture* t) {_albedo = t;}
    inline void SetAlbedoTextureStage(const unsigned int& s) {_albedoStage = s;}

    inline ITexture* GetEmissiveTexture();
    inline unsigned int GetEmissiveTextureStage();

    inline void SetEmissiveTexture(ITexture* t) {_emissive = t;}
    inline void SetEmissiveTextureStage(const unsigned int& s) {_emissiveStage = s;}

    inline ITexture* GetEnvironmentTexture();
    inline unsigned int GetEnvironmentTextureStage();

    inline void SetEnvironmentTexture(ITexture* t) {_environment = t;}
    inline void SetEnvironmentTextureStage(const unsigned int& s) {_environmentStage = s;}

    inline bool IsTwoSided();
    inline void SetTwoSided(const bool& ts);

    MaterialPass(Material* mat);
    ~MaterialPass();
protected:
    Material* _parent;
    Shader* _shader;

    bool _twoSided;

    float _alphaDiscard; //discard alpha clamp value; eg ( value = 0.7; if( alpha < 0.7 ){ discard; } )
    MR::IShaderUniform* _uniform_a_discard;

    MaterialFlag _flag;

    ITexture* _albedo;
    int _albedoStage; //GL_TEXTURE0 is 0 / GL_TEXTURE1 i s1 / GL_TEXTURE2 is 2 etc
    MR::IShaderUniform* _albedo_sample;

    ITexture* _emissive;
    int _emissiveStage;
    MR::IShaderUniform* _emissive_sample;

    ITexture* _environment;
    int _environmentStage;
    MR::IShaderUniform* _environment_sample;

    glm::vec4* _color; //effects by light
    MR::IShaderUniform* _uniform_color;

    glm::vec4* _emissive_color; // result = _color * light + _emissive_color;
    MR::IShaderUniform* _uniform_emissive_color;
};

class Material {
public:
    inline std::string GetName() { return name; }
    inline MaterialPass** GetPasses() { return materialPasses.data(); }
    inline MaterialPass* GetPass(const unsigned int & i) { return materialPasses[i]; }
    inline MaterialManager* GetManager() { return manager; }
    inline std::vector<MaterialPass*>::size_type GetPassesNum() { return materialPasses.size(); }

    inline void AddPass(MaterialPass* p){ materialPasses.push_back(p); }

    inline MaterialPass* CreatePass(){
        MaterialPass* p = new MaterialPass(this);
        materialPasses.push_back(p);
        return p;
    }

    Material(MaterialManager* mgr, const std::string& Name);

protected:
    std::string name;
    std::vector<MaterialPass*> materialPasses;
    MaterialManager* manager;
};

class MaterialManager {
public:
    inline unsigned char ActivedFlag();
    inline void ActiveFlag(const unsigned char & f);

    static MaterialManager* Instance() {
        static MaterialManager* m = new MaterialManager();
        return m;
    }

protected:
    unsigned char _flag; //actived flag
};

}

MR::MaterialFlag MR::MaterialPass::GetFlag() {
    return _flag;
}

MR::MaterialFlag* MR::MaterialPass::GetFlagPtr(){
    return &_flag;
}

MR::Material* MR::MaterialPass::GetMaterial() {
    return _parent;
}

MR::Shader* MR::MaterialPass::GetShader() {
    return _shader;
}

MR::ITexture* MR::MaterialPass::GetAlbedoTexture() {
    return _albedo;
}

unsigned int MR::MaterialPass::GetAlbedoTextureStage() {
    return _albedoStage;
}

MR::ITexture* MR::MaterialPass::GetEmissiveTexture() {
    return _emissive;
}

unsigned int MR::MaterialPass::GetEmissiveTextureStage() {
    return _emissiveStage;
}

MR::ITexture* MR::MaterialPass::GetEnvironmentTexture() {
    return _environment;
}

unsigned int MR::MaterialPass::GetEnvironmentTextureStage() {
    return _environmentStage;
}

bool MR::MaterialPass::IsTwoSided() {
    return _twoSided;
}

void MR::MaterialPass::SetTwoSided(const bool& ts) {
    _twoSided = ts;
}

unsigned char MR::MaterialManager::ActivedFlag() {
    return _flag;
}

void MR::MaterialManager::ActiveFlag(const unsigned char & f) {
    _flag = f;
}

#endif
