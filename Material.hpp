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
class Shader;
class ShaderUniform;
class Texture;
class RenderContext;
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
    void Use(RenderContext* rc);
    inline MaterialFlag& GetFlag() { return flag; }
    inline MaterialFlag* GetFlagP(){ return &flag;}

    inline Material* GetMaterial() { return parent; }
    inline Texture* GetDiffuseTexture() { return diffuseTexture; }
    inline unsigned int GetDiffuseTextureStage() { return diffuseTextureStage; }
    inline Shader* GetShader() { return shader; }
    inline bool IsTwoSided() { return twoSided; }
    inline void SetDiffuseTexture(Texture* t) {diffuseTexture = t;}
    inline void SetDiffuseTextureStage(const unsigned int& s) { diffuseTextureStage = s; }
    void SetShader(Shader* sh);
    inline void SetTwoSided(const bool& ts) { twoSided = ts; }

    MaterialPass(Material* mat);
    MaterialPass(Material* mat, Texture* dTex, unsigned int dTexStage, Shader* sh);

protected:
    Material* parent;
    Texture* diffuseTexture;
    unsigned int diffuseTextureStage; //GL_TEXTURE0/GL_TEXTURE1/GL_TEXTURE2 etc
    Shader* shader; //pointer to shader
    bool twoSided;
    MaterialFlag flag;
    bool alphaDiscard;
    glm::vec4 _ambientLight;
    MR::ShaderUniform* _u_ambientLight;
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
    inline unsigned char ActivedFlag() const { return flag; }
    inline void ActiveFlag(const unsigned char & f) { flag = f; }

    static MaterialManager* Instance() {
        static MaterialManager* m = new MaterialManager();
        return m;
    }

protected:
    unsigned char flag; //actived flag
};

}

#endif
