#pragma once

#ifndef _MR_MATERIAL_H_
#define _MR_MATERIAL_H_

#include "MaterialInterfaces.hpp"
#include <vector>

namespace MR {

class MaterialPass : public IMaterialPass {
public:
    inline void SetFlag(const MaterialFlag& flag) override { if( !(_flag == flag) ) { _flag = flag; OnFlagChanged(this, flag); } }
    inline MaterialFlag GetFlag() override { return _flag; }

    inline void SetShaderProgram(IShaderProgram* sp) override { if(sp != _program) { _program = sp; OnShaderProgramChanged(this, sp); } }
    inline IShaderProgram* GetShaderProgram() { return _program; }

    void SetTexture(const TextureType& type, ITexture* tex) override;
    ITexture* GetTexture(const TextureType& type) override;

    inline void SetTwoSided(const bool& s) { if(s != _two_sided){ _two_sided = s; OnTwoSidedStateChanged(this, s); } }
    inline bool GetTwoSided() { return _two_sided; }

    inline void SetColor(const glm::vec4& color) { if(_color != color){ _color = color; OnColorChanged(this, color); } }
    inline glm::vec4 GetColor() { return _color; }

    MaterialPass();
    MaterialPass(const MaterialFlag& f, IShaderProgram* sp, const bool& twoSided, const glm::vec4& c);
    virtual ~MaterialPass();

    //Usable
    bool Use() override;
protected:
    MaterialFlag _flag;
    IShaderProgram* _program;
    MR::Dictionary<TextureType, MR::ITexture*, nullptr> _textures;
    bool _two_sided;
    glm::vec4 _color;
};

class Material : public IMaterial {
public:
    MR::TStaticArray<IMaterialPass*> GetAllPasses() override { return MR::TStaticArray<IMaterialPass*>(&_passes[0], _passes.size(), false); }
    IMaterialPass* GetPass(const size_t& index) override { return _passes[index];}
    MR::TStaticArray<IMaterialPass*> GetPasses(const MaterialFlag& flag) override;

    IMaterialPass* CreatePass(const MaterialFlag& flag) override;
    inline void AddPass(IMaterialPass* pass) override { _passes.push_back(pass); }
    inline void InsertPass(IMaterialPass* pass, const size_t& index) override { _passes.insert(_passes.begin()+index, pass); }
    void RemovePass(IMaterialPass* pass) override;
    void RemovePass(const size_t& index) override;

    void SetShaderProgram(const MaterialFlag& flag, IShaderProgram* sp) override;
    void SetTexture(const MaterialFlag& flag, const IMaterialPass::TextureType& type, ITexture* tex) override;
    void SetTwoSided(const MaterialFlag& flag, const bool& s) override;
    void SetColor(const MaterialFlag& flag, const glm::vec4& color) override;

    Material();
    Material(MR::TStaticArray<IMaterialPass*> passes);
    virtual ~Material();

    static IMaterial* Create();
protected:
    std::vector<IMaterialPass*> _passes;
};

}

#endif
