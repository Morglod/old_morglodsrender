#pragma once

#ifndef _MR_RENDER_MANAGER_H_
#define _MR_RENDER_MANAGER_H_

#include "pre.hpp"
#include "Events.hpp"

namespace MR {
class Camera;
class Entity;
class Model;
class Mesh;
class Shader;
class GeometryBuffer;
class Material;
class Texture;
class TextureSettings;

class RenderContext {
public:
    MR::Event<Material*> OnDefaultMaterialChanged;
    MR::Event<bool> OnDefaultMaterialUsingStateChanged;

    Camera* camera;

    inline GeometryBuffer* GetCurrentGeometryBuffer() { return _gb; }
    inline Shader* GetCurrentShader() { return _sh; }
    inline GLuint GetCurrentTexture(GLenum stage) { return _tx[stage-GL_TEXTURE0]; }
    inline GLuint* GetCurrentTextures() { return &_tx[0]; }

    void UseGeometryBuffer(GeometryBuffer* gb);
    void UseShader(Shader* sh);
    void ActiveTextureUnit(const GLenum & u);
    void BindTexture(const GLuint& tx, GLenum texStage);
    void BindTexture(Texture* tx, const GLenum& texStage);
    void UseTextureSettings(TextureSettings* ts, GLenum texStage);
    void SetDefaultMaterial(Material* mat);
    void SetViewport(const unsigned short & x, const unsigned short & y, const unsigned short & width, const unsigned short & height);
    void UseDefaultMaterialState(bool s);
    void DrawGeometryBuffer(GeometryBuffer* gb);
    void DrawGeomWithMaterial(glm::mat4* model_mat, MR::GeometryBuffer* g, MR::Material* mat);
    void DrawEntity(MR::Entity* ent);

    virtual bool Init();

    //Calculates frames per second
    //timeType is data type of time in calculations
    template<typename timeType>
    static unsigned short inline FPS(){
        static timeType lastTime = 0.0;
        static timeType time = 0.0;
        static unsigned short frames = 0;
        static unsigned short lastFps = 0;

        timeType currentTime = (timeType)glfwGetTime();
        time += currentTime-lastTime;
        lastTime = currentTime;
        ++frames;
        if(time > 1){
            lastFps = frames;
            time -= 1.0;
            frames = 0;
        }

        return lastFps;
    }

    RenderContext();
    ~RenderContext();

protected:
    GeometryBuffer* _gb = nullptr;
    Shader* _sh = nullptr;

    Material* defaultMaterial = nullptr;
    bool useDefaultMaterial = true;

    //Viewport---
    unsigned short vp_x = 0;
    unsigned short vp_y = 0;
    unsigned short vp_w = 0;
    unsigned short vp_h = 0;
    //-----------

    //Textures--
    GLuint activedTextureUnit = GL_TEXTURE0;
    GLuint _tx[GL_TEXTURE31-GL_TEXTURE0+1];
    TextureSettings* _ts[GL_TEXTURE31-GL_TEXTURE0+1];
    //----------
};

}

#endif
