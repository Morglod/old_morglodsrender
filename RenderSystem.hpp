#pragma once

#ifndef _MR_RENDER_SYSTEM_H_
#define _MR_RENDER_SYSTEM_H_

#include "Types.hpp"

#include <glm/glm.hpp>

namespace MR {

class ITexture;
class ITextureSettings;
class IViewport;
class IRenderWindow;
class IVertexFormat;
class IIndexFormat;
class IGeometry;
class IShader;

class Camera;
class Entity;
class Material;
class MaterialPass;
class RenderTarget;

/* LOW LEVEL METHODS */
class IRenderSystem {
public:
    virtual bool Init(IRenderWindow* window) = 0;
    virtual void Shutdown() = 0;

    /* Is Initializated and not shutdowned */
    virtual bool Alive() = 0;

    virtual void SetViewport(const unsigned short & x, const unsigned short & y, const unsigned short & width, const unsigned short & height) = 0;
    virtual void SetViewport(IViewport* v) = 0;
    virtual IViewport* GetViewport() = 0;

    virtual void UseCamera(Camera* cam) = 0;
    virtual Camera* GetCamera() = 0;

    virtual void UseShader(IShader* shader) = 0;
    virtual IShader* GetShader() = 0;

    virtual unsigned int FreeTextureUnit() = 0;
    virtual unsigned int GetTexture(const unsigned int& unit) = 0;
    virtual void BindTexture(ITexture* tex, const unsigned int& unit) = 0;
    virtual void BindTexture(const unsigned int& target, const unsigned int& gpuHandle, const unsigned int& unit) = 0;
    virtual void UnBindTexture(const unsigned int& unit) = 0;
    virtual void BindTextureSettings(ITextureSettings* ts, const unsigned int& unit) = 0;

    virtual void BindVertexFormat(IVertexFormat* format) = 0;
    virtual void BindIndexFormat(IIndexFormat* format) = 0;

    virtual void BindRenderTarget(RenderTarget* t) = 0;
    virtual void UnBindRenderTarget() = 0;

    virtual void DrawGeometry(IGeometry* gb) = 0;
    virtual void DrawGeomWithMaterial(glm::mat4* model_mat, MR::IGeometry* g, MR::Material* mat) = 0;
    virtual void DrawGeomWithMaterialPass(glm::mat4* model_mat, MR::IGeometry* g, MR::MaterialPass* mat_pass) = 0;

    virtual void DrawEntity(Entity* ent) = 0;
};

/** BASE RENDER SYSTEM CALLS THIS METHODS AUTOMATICALLY **/

/* New RenderSystem should be registered and unregistered before deleting */
void RegisterRenderSystem(IRenderSystem* rs);
void UnRegisterRenderSystem(IRenderSystem* rs);

/* Should be called after Init and Shutdown methods */
void RenderSystemInitializated(IRenderSystem* rs);
void RenderSystemShutdowned(IRenderSystem* rs);

/* Check if any render system is alive */
bool AnyRenderSystemAlive();

/** -- **/

/* BASE RENDER SYSTEM IMPLEMENTION */
class RenderSystem : public MR::IRenderSystem, public MR::Object {
public:
    inline std::string ToString() override { return "RenderSystem(base)"; }

    bool Init(IRenderWindow* window) override;
    void Shutdown() override;

    inline bool Alive() override { return _alive; }

    void SetViewport(const unsigned short & x, const unsigned short & y, const unsigned short & width, const unsigned short & height) override;
    void SetViewport(IViewport* vp) override;
    inline IViewport* GetViewport() override { return _viewport; }

    void UseCamera(Camera* cam) override;
    inline Camera* GetCamera() override { return _cam; }

    void UseShader(IShader* shader) override;
    inline IShader* GetShader() override { return _shader; }

    unsigned int FreeTextureUnit() override;
    inline unsigned int GetTexture(const unsigned int& unit) override {
        if(_textures.size() >= unit) return 0;
        return _textures[unit].gl_texture;
    }
    void BindTexture(ITexture* tex, const unsigned int& unit) override;
    void BindTexture(const unsigned int& target, const unsigned int& gpuHandle, const unsigned int& unit) override;
    void UnBindTexture(const unsigned int& unit) override;
    void BindTextureSettings(ITextureSettings* ts, const unsigned int& unit) override;

    void BindVertexFormat(IVertexFormat* format) override;
    void BindIndexFormat(IIndexFormat* format) override;

    void BindRenderTarget(RenderTarget* t) override;
    void UnBindRenderTarget() override;

    void DrawGeometry(IGeometry* gb) override;
    void DrawGeomWithMaterial(glm::mat4* model_mat, MR::IGeometry* g, MR::Material* mat) override;
    void DrawGeomWithMaterialPass(glm::mat4* model_mat, MR::IGeometry* g, MR::MaterialPass* mat_pass) override;

    void DrawEntity(MR::Entity* ent) override;

    RenderSystem();
    ~RenderSystem();
protected:
    bool _init;
    bool _alive;
    bool _glew;
    bool _glfw;

    IViewport* _viewport;

    class BindedTexture {
    public:
        ITexture* tex = nullptr;
        unsigned int gl_texture = 0; //if = 0, texture not binded
        bool isArray = false;
        ITextureSettings* settings;
    };

    unsigned int _nextFreeUnit;
    std::vector<BindedTexture> _textures;

    inline void _AllocateTextureUnits(const unsigned int& unit){
        while(unit >= _textures.size()){
            _textures.push_back(BindedTexture());
        }
    }

    IVertexFormat* _vformat;
    IIndexFormat* _iformat;

    Camera* _cam;

    IShader* _shader;

    bool _useDefaultMaterial;
    Material* _defaultMaterial;

    RenderTarget* _renderTarget;
};

}

#endif // _MR_RENDER_SYSTEM_H_
