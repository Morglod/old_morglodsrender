#pragma once

#ifndef _MR_LIGHT_H_
#define _MR_LIGHT_H_

#include "Config.hpp"
#include "Types.hpp"
#include "Shader.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace MR {

class ILightSource;
class LightSource;
class LightManager;
class IShader;
class ShaderManager;

class IShadowMap {
public:
    virtual ILightSource* GetLight() = 0;
    virtual int GetWidth() = 0;
    virtual int GetHeight()= 0;
    virtual void BeginCapture(RenderContext* context, ILightSource* light) = 0;
    virtual void EndCapture(RenderContext* context, ILightSource* light) = 0;
    virtual void SetProjection(const glm::mat4& p) = 0;
    virtual void SetView(const glm::mat4& v) = 0;
};

class ShadowMapDepth : public Object, public IShadowMap {
public:
    inline ILightSource* GetLight() override { return _captured_from; }
    inline int GetWidth() override { return _w; }
    inline int GetHeight() override { return _h; }

    void BeginCapture(RenderContext* context, ILightSource* light) override;
    void EndCapture(RenderContext* context, ILightSource* light) override;

    void SetProjection(const glm::mat4& p) override;
    void SetView(const glm::mat4& v) override;

    ShadowMapDepth(ILightSource* light, const int& width, const int& height);
    virtual ~ShadowMapDepth();
protected:
    glm::mat4 _projection, _view;
    unsigned int _gl_texture, _gl_framebuffer;
    ILightSource* _captured_from;
    int _w, _h;
};

class ShadowMapColor : public Object, public IShadowMap {
public:
    inline ILightSource* GetLight() override { return _captured_from; }
    inline int GetWidth() override { return _w; }
    inline int GetHeight() override { return _h; }

    void BeginCapture(RenderContext* context, ILightSource* light) override;
    void EndCapture(RenderContext* context, ILightSource* light) override;

    void SetProjection(const glm::mat4& p) override;
    void SetView(const glm::mat4& v) override;

    ShadowMapColor(ILightSource* light, const int& width, const int& height);
    virtual ~ShadowMapColor();
protected:
    glm::mat4 _projection, _view;
    ILightSource* _captured_from;
    unsigned int _gl_texture, _gl_framebuffer, _gl_renderbuffer;
    int _w, _h;
};

class ILightSource {
public:
    enum class Type : unsigned char {
        SpotLight = 0,
        PointLight = 1
    };

    virtual void SetShadowMap(IShadowMap* map) = 0;
    virtual glm::vec3 GetPos() = 0;
    virtual glm::vec3 GetDir() = 0;
    virtual LightManager* GetManager() = 0;
};

class LightSource : public Object, public ILightSource {
public:
    void SetShadowMap(IShadowMap* map) override;
    glm::vec3 GetPos() override;
    glm::vec3 GetDir() override;
    inline LightManager* GetManager() override {return _manager;}

    ///TODO
    static ILightSource* CreateSpotLight(const glm::vec3& pos, const glm::vec4& color, const float& radius);
    static ILightSource* CreatePointLight(const glm::vec3& pos, const glm::vec4& color, const float& radius);

    virtual ~LightSource();
private:
    LightSource() : Object() {}

protected:
    IShadowMap* _captured;
    LightManager* _manager;
    ILightSource::Type _type;
    glm::vec3 _pos;
    glm::vec3 _dir;
    glm::vec4 _color; //r,g,b,multiply
    float _radius;
};

class LightManager : public Object {
public:
    MR::IShader* GetDepth() { return _depth; }

    LightManager(ShaderManager* sm);
    ~LightManager();

    static LightManager* Instance(MR::ShaderManager* manager){
        if(static_instance == 0) static_instance = new LightManager(manager);
        return static_instance;
    }

    static LightManager* Instance(){
        if(static_instance == 0) static_instance = new LightManager(MR::ShaderManager::Instance());
        return static_instance;
    }
protected:
    static LightManager* static_instance;

    MR::ShaderManager* _shader_manager;
    MR::IShader* _shadow_color;
    MR::IShader* _shadow_depth;
    MR::IShader* _depth;
    int* _shadow_color_map_unit;
    int* _shadow_depth_map_unit;
    glm::mat4* _shadow_color_mat_light;
    glm::mat4* _shadow_depth_mat_light;
    glm::vec3* _shadow_color_vec3_light_pos;
    glm::vec3* _shadow_depth_vec3_light_pos;
    glm::vec3* _shadow_color_vec3_light_dir;
    glm::vec3* _shadow_depth_vec3_light_dir;
};

}

#endif // _MR_LIGHT_H_
