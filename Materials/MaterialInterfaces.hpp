#pragma once

#ifndef _MR_MATERIAL_INTERFACES_H_
#define _MR_MATERIAL_INTERFACES_H_

#include "../Types.hpp"
#include "../Utils/Containers.hpp"
#include "../Utils/Events.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

#include <vector>

namespace MR {

class IShaderProgram;
class ITexture;
struct MaterialFlag;

struct MaterialFlag {
public:
    bool always; //use material pass always (flag wouldn't change anything)
    unsigned char flag; //if always==false, use this pass only if this flag is On
    MaterialFlag() : always(false), flag(0) {}
    MaterialFlag(bool Always, unsigned char Flag) : always(Always), flag(Flag) {}

    inline static MaterialFlag Default(){return MaterialFlag(false, 0);} //default rendering state
    inline static MaterialFlag ShadowMap(){return MaterialFlag(false, 1);} //drawing to shadow map
    inline static MaterialFlag ToTexture(){return MaterialFlag(false, 2);} //drawing to render target

    inline bool operator == (const MaterialFlag& flag_) {
        return (always == flag_.always) && (flag == flag_.flag);
    }
};

class IMaterialPass : public MR::Usable {
public:
    enum TextureType {
        TT_Albedo = 0,
        TT_Emission = 1,
        TT_OpacityMask = 2
    };

    MR::EventListener<IMaterialPass*, const MaterialFlag&> OnFlagChanged;
    MR::EventListener<IMaterialPass*, IShaderProgram*> OnShaderProgramChanged;
    MR::EventListener<IMaterialPass*, const TextureType&, ITexture*> OnTextureChanged;
    MR::EventListener<IMaterialPass*, const bool&> OnTwoSidedStateChanged;
    MR::EventListener<IMaterialPass*, const glm::vec4&> OnColorChanged;

    virtual void SetFlag(const MaterialFlag& flag) = 0;
    virtual MaterialFlag GetFlag() = 0;

    virtual void SetShaderProgram(IShaderProgram* sp) = 0;
    virtual IShaderProgram* GetShaderProgram() = 0;

    virtual void SetTexture(const TextureType& type, ITexture* tex) = 0;
    virtual ITexture* GetTexture(const TextureType& type) = 0;

    virtual void SetTwoSided(const bool& s) = 0;
    virtual bool GetTwoSided() = 0;

    virtual void SetColor(const glm::vec4& color) = 0;
    virtual glm::vec4 GetColor() = 0;

    virtual ~IMaterialPass() {}
};

class IMaterial {
public:
    //

    virtual MR::StaticArray<IMaterialPass*> GetAllPasses() = 0;
    virtual IMaterialPass* GetPass(const size_t& index) = 0;
    virtual MR::StaticArray<IMaterialPass*> GetPasses(const MaterialFlag& flag) = 0;

    //

    virtual IMaterialPass* CreatePass(const MaterialFlag& flag) = 0;
    virtual void AddPass(IMaterialPass* pass) = 0;
    virtual void InsertPass(IMaterialPass* pass, const size_t& index) = 0;
    virtual void RemovePass(IMaterialPass* pass) = 0;
    virtual void RemovePass(const size_t& index) = 0;

    // Sets to all passes of MaterialFlag

    virtual void SetShaderProgram(const MaterialFlag& flag, IShaderProgram* sp) = 0;
    virtual void SetTexture(const MaterialFlag& flag, const IMaterialPass::TextureType& type, ITexture* tex) = 0;
    virtual void SetTwoSided(const MaterialFlag& flag, const bool& s) = 0;
    virtual void SetColor(const MaterialFlag& flag, const glm::vec4& color) = 0;

    virtual ~IMaterial() {}
};

}

#endif // _MR_MATERIAL_INTERFACES_H_
