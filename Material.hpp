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

class IShader;
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
    inline IShader* GetShader();
    virtual void SetShader(IShader* sh);

    inline ITexture* GetAmbientTexture() { return _ambient; }
    inline unsigned int GetAmbientTextureUnit() { return _ambientUnit; }

    inline void SetAmbientTexture(ITexture* t) {_ambient = t;}
    inline void SetAmbientTextureUnit(const unsigned int& s) {_ambientUnit = s;}

    inline ITexture* GetDiffuseTexture() { return _diffuse; }
    inline unsigned int GetDiffuseTextureUnit() { return _diffuseUnit; }

    inline void SetDiffuseTexture(ITexture* t) {_diffuse = t;}
    inline void SetDiffuseTextureUnit(const unsigned int& s) {_diffuseUnit = s;}

    inline ITexture* GetOpacityTexture() { return _opacity; }
    inline unsigned int GetOpacityTextureUnit() { return _opacityUnit; }

    inline void SetOpacityTexture(ITexture* t) {_opacity = t;}
    inline void SetOpacityTextureUnit(const unsigned int& s) {_opacityUnit = s;}

    inline bool IsTwoSided();
    inline void SetTwoSided(const bool& ts);

    MaterialPass(Material* mat);
    ~MaterialPass();
protected:
    Material* _parent;
    IShader* _shader;

    bool _twoSided;

    MaterialFlag _flag;

    ITexture* _ambient;
    int _ambientUnit; //GL_TEXTURE0 is 0 / GL_TEXTURE1 i s1 / GL_TEXTURE2 is 2 etc
    MR::IShaderUniform* _ambient_sampler;

    ITexture* _diffuse;
    int _diffuseUnit; //GL_TEXTURE0 is 0 / GL_TEXTURE1 i s1 / GL_TEXTURE2 is 2 etc
    MR::IShaderUniform* _diffuse_sampler;

    ITexture* _opacity;
    int _opacityUnit;
    MR::IShaderUniform* _opacity_sampler;

    glm::vec4* _color; //light filter
    MR::IShaderUniform* _uniform_color;
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

MR::IShader* MR::MaterialPass::GetShader() {
    return _shader;
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
