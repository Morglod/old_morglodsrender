#pragma once

#ifndef _MR_RENDER_MANAGER_H_
#define _MR_RENDER_MANAGER_H_

#include "Config.hpp"
#include "Events.hpp"
#include "Texture.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace MR {
class Camera;
class Entity;
class Model;
class Mesh;
class Shader;
class GeometryBuffer;
class Material;
class Texture;
class TextureArray;
class TextureSettings;

class RenderContext {
    friend class Texture;
public:
    MR::Event<Material*> OnDefaultMaterialChanged;
    MR::Event<bool> OnDefaultMaterialUsingStateChanged;

    Camera* camera;

    inline GeometryBuffer* GetCurrentGeometryBuffer() { return _gb; }
    inline Shader* GetCurrentShader() { return _sh; }
    inline unsigned int GetCurrentTexture(unsigned int stage) { return _tx[stage-0x84C0]; }
    inline unsigned int* GetCurrentTextures() { return &_tx[0]; }

    void UseGeometryBuffer(GeometryBuffer* gb);
    void UseShader(Shader* sh);
    void ActiveTextureUnit(const unsigned int & u);
    void BindTexture(const unsigned int& tx, unsigned int texStage);
    void BindTexture(Texture* tx, unsigned int texStage);
    void UseTextureSettings(TextureSettings::Ptr ts, unsigned int texStage);
    void SetDefaultMaterial(Material* mat);
    void SetViewport(const unsigned short & x, const unsigned short & y, const unsigned short & width, const unsigned short & height);
    void UseDefaultMaterialState(bool s);
    void DrawGeometryBuffer(GeometryBuffer* gb);
    void DrawGeomWithMaterial(glm::mat4* model_mat, MR::GeometryBuffer* g, MR::Material* mat);
    void DrawEntity(MR::Entity* ent);

    virtual bool Init();

    //Calculates frames per second
    //timeType is data type of time in calculations
    static unsigned short FPS();

    //Call it only one time each frame or calculate it in your code
    static float Delta();

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
    unsigned int activedTextureUnit = 0x84C0; //GL_TEXTURE0
    unsigned int _tx[32];
    unsigned int _tai[32]; //texture array index
    bool _tab[32]; //is texture array used
    TextureSettings::Ptr _ts[32];

    ///TODO---------------------------------------------------------------------------------------------------------------------
    int _diffuseTextureUnit = 0;
    //----------
};

}

#endif
