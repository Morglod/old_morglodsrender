#pragma once

#ifndef _MR_MATERIAL_H_
#define _MR_MATERIAL_H_

#include "Config.hpp"
#include "Utils/Singleton.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

#include <string>
#include <vector>

namespace MR {

class RenderContext;

class IShaderProgram;
class IShaderUniform;

class ITexture;

class IRenderSystem;

class Material;
class MaterialManager;


class MaterialPass {
public:
    virtual bool Use();
    virtual void UnUse();

    inline MaterialFlag GetFlag();
    inline MaterialFlag* GetFlagPtr();

    inline Material* GetMaterial();
    inline IShaderProgram* GetShader();
    virtual void SetShader(IShaderProgram* sh);

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
    IShaderProgram* _shader;

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

    inline virtual void AddPass(MaterialPass* p){ materialPasses.push_back(p); }

    inline virtual MaterialPass* CreatePass(){
        MaterialPass* p = new MaterialPass(this);
        materialPasses.push_back(p);
        return p;
    }

    /** SETS TO ALL PASSES **/
    virtual void SetShader(IShaderProgram* sh);

    virtual void SetAmbientTexture(ITexture* t);
    virtual void SetAmbientTextureUnit(const unsigned int& s);

    virtual void SetDiffuseTexture(ITexture* t);
    virtual void SetDiffuseTextureUnit(const unsigned int& s);

    virtual void SetOpacityTexture(ITexture* t);
    virtual void SetOpacityTextureUnit(const unsigned int& s);

    virtual void SetTwoSided(const bool& ts);
    /** **/

    Material(MaterialManager* mgr, const std::string& Name);
    virtual ~Material() {}
protected:
    std::string name;
    std::vector<MaterialPass*> materialPasses;
    MaterialManager* manager;
};

class MaterialManager : public Singleton<MaterialManager> {
public:
    inline unsigned char GetActivedFlag();
    inline void ActiveFlag(const unsigned char & f);
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

MR::IShaderProgram* MR::MaterialPass::GetShader() {
    return _shader;
}

bool MR::MaterialPass::IsTwoSided() {
    return _twoSided;
}

void MR::MaterialPass::SetTwoSided(const bool& ts) {
    _twoSided = ts;
}

unsigned char MR::MaterialManager::GetActivedFlag() {
    return _flag;
}

void MR::MaterialManager::ActiveFlag(const unsigned char & f) {
    _flag = f;
}

#endif
