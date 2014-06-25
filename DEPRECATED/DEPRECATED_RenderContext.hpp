#pragma once

#ifndef _MR_RENDER_MANAGER_H_
#define _MR_RENDER_MANAGER_H_

#include "Config.hpp"
#include "Events.hpp"
#include "Texture.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

#include <stack>

namespace MR {
class Camera;
class Entity;
class Model;
class Mesh;
class Shader;
class IGeometry;
class Material;
class MaterialPass;
class Texture;
class TextureArray;
class TextureSettings;
class IVertexFormat;
class IIndexFormat;

class RenderContext {
    friend class Texture;
public:
    MR::EventListener<Camera*> OnCameraUsed;
    //MR::EventListener<GeometryBuffer*> OnGeometryBufferUsed;
    MR::EventListener<Shader*> OnShaderUsed;
    MR::EventListener<const unsigned int&> OnTextureUnitActived;

    MR::EventListener<Material*> OnDefaultMaterialChanged;
    MR::EventListener<bool> OnDefaultMaterialUsingStateChanged;

    inline Camera* GetCurrentCamera();
    //inline GeometryBuffer* GetCurrentGeometryBuffer();
    inline Shader* GetCurrentShader();
    inline unsigned int GetCurrentTexture(unsigned int stage);
    inline unsigned int* GetCurrentTextures();

    void UseCamera(Camera* cam);
    //void UseGeometryBuffer(GeometryBuffer* gb);
    void UseShader(Shader* sh);
    void UseDefaultMaterial(const bool& s);
    void UseTextureSettings(TextureSettings::Ptr ts, const unsigned int& texUnit);

    void ActiveTextureUnit(const unsigned int & texUnit);

    void BindTexture(const Texture::Target& target, const unsigned int& tx, const unsigned int& texUnit);
    void BindTexture(Texture* tx, const unsigned int& texUnit);
    void BindVertexFormat(IVertexFormat* format);
    void BindIndexFormat(IIndexFormat* format);

    void SetDefaultMaterial(Material* mat);
    void SetViewport(const unsigned short & x, const unsigned short & y, const unsigned short & width, const unsigned short & height);

    void DrawGeometry(IGeometry* gb);
    void DrawGeomWithMaterial(glm::mat4* model_mat, MR::IGeometry* g, MR::Material* mat);
    void DrawGeomWithMaterialPass(glm::mat4* model_mat, MR::IGeometry* g, MR::MaterialPass* mat_pass);

    void DrawEntity(MR::Entity* ent);
    void DrawEntity(MR::Entity** ent_list, const unsigned int& num, const bool& instancing = false);

    virtual bool Init();

    //Calculates frames per second
    //timeType is data type of time in calculations
    static unsigned short FPS();

    //Call it only one time each frame or calculate it in your code
    static float Delta();

    static inline bool Alive();

    RenderContext();
    ~RenderContext();

protected:
    bool _is_alive;

    Camera* _cam = nullptr;
    //GeometryBuffer* _gb = nullptr;
    IVertexFormat* _vformat = nullptr;
    IIndexFormat* _iformat = nullptr;
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
    unsigned int activedTextureUnit = 0; //GL_TEXTURE0
    unsigned int _tx[32];
    unsigned int _tai[32]; //texture array index
    bool _tab[32]; //is texture array used
    TextureSettings::Ptr _ts[32];

    //std::stack<IRenderTechnique*> _render_technique;

    static RenderContext* _instance;

    ///TODO---------------------------------------------------------------------------------------------------------------------
    int _diffuseTextureUnit = 0;
    //----------
};

}

bool MR::RenderContext::Alive(){
    return ( (RenderContext::_instance != nullptr) && (RenderContext::_instance->_is_alive) );
}

MR::Camera* MR::RenderContext::GetCurrentCamera(){
    return _cam;
}

/*MR::GeometryBuffer* MR::RenderContext::GetCurrentGeometryBuffer(){
    return _gb;
}*/

MR::Shader* MR::RenderContext::GetCurrentShader(){
    return _sh;
}

unsigned int MR::RenderContext::GetCurrentTexture(unsigned int stage){
    return _tx[stage];
}

unsigned int* MR::RenderContext::GetCurrentTextures(){
    return &_tx[0];
}

#endif
