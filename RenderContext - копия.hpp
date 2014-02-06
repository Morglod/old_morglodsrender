#pragma once

#ifndef _MR_RENDER_MANAGER_H_
#define _MR_RENDER_MANAGER_H_

#include "pre.hpp"

#include "GeometryBuffer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Events.hpp"
#include "Camera.hpp"
#include "Material.hpp"

#include "Mesh.hpp"
#include "Model.hpp"
#include "Entity.hpp"

namespace MR {
class Camera;
class Entity;
class Model;
class Mesh;

class RenderContext {
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

public:
    MR::Event<Material*> OnDefaultMaterialChanged;
    MR::Event<bool> OnDefaultMaterialUsingStateChanged;

    Camera* camera;

    inline GeometryBuffer* GetCurrentGeometryBuffer() { return _gb; }
    inline Shader* GetCurrentShader() { return _sh; }
    inline GLuint GetCurrentTexture(GLenum stage) { return _tx[stage-GL_TEXTURE0]; }
    inline GLuint* GetCurrentTextures() { return &_tx[0]; }

    inline void UseGeometryBuffer(GeometryBuffer* gb) {
        if(gb != _gb) {
            _gb = gb;
            _gb->Bind();
        }
    }

    inline void UseShader(Shader* sh) {
        _sh = sh;
        if(_sh != nullptr) _sh->Use();
    }

    inline void ActiveTextureUnit(const GLenum & u){
        if(activedTextureUnit != u) {
            glActiveTexture(u);
            activedTextureUnit = u;
        }
    }

    inline void BindTexture(const GLuint& tx, GLenum texStage) {
        ActiveTextureUnit(texStage);
        texStage -= GL_TEXTURE0;
        if( _tx[texStage] != tx ) {
            _tx[texStage] = tx;
            glBindTexture(GL_TEXTURE_2D, tx);
        }
    }

    inline void BindTexture(Texture* tx, const GLenum& texStage) {
        BindTexture(tx->GetGLTexture(), texStage);
        UseTextureSettings(tx->GetSettings(), texStage);
    }

    inline void UseTextureSettings(TextureSettings* ts, GLenum texStage){
        texStage -= GL_TEXTURE0;
        if( _ts[texStage] != ts ) {
            _ts[texStage] = ts;
            if(ts == nullptr) glBindSampler(texStage, 0);
            else glBindSampler(texStage, ts->GetGLSampler());
        }
    }

    inline void SetDefaultMaterial(Material* mat) {
        if(defaultMaterial != mat){
            defaultMaterial = mat;
            OnDefaultMaterialChanged(this, mat);
        }
    }

    inline void SetViewport(const unsigned short & x, const unsigned short & y, const unsigned short & width, const unsigned short & height){
        if( (vp_x != x) || (vp_y != y) || (vp_w != width) || (vp_h != height) ){
            glViewport(x, y, width, height);
            vp_x = x;
            vp_y = y;
            vp_w = width;
            vp_h = height;
        }
    }

    inline void UseDefaultMaterialState(bool s) {
        if(useDefaultMaterial != s){
            useDefaultMaterial = s;
            OnDefaultMaterialUsingStateChanged(this, s);
        }
    }

    inline void DrawGeometryBuffer(GeometryBuffer* gb) {
        UseGeometryBuffer(gb);
        _gb->Draw();
    }

    inline void DrawGeomWithMaterial(glm::mat4* model_mat, MR::GeometryBuffer* g, MR::Material* mat){
        //UseGeometryBuffer(g);
        if(mat){
            for(unsigned short i = 0; i < mat->GetPassesNum(); ++i){
                camera->SetModelMatrix(model_mat);
                mat->GetPass(i)->Use(this);
                DrawGeometryBuffer(g);
            }
        } else {
            if(useDefaultMaterial && defaultMaterial){
                for(unsigned short i = 0; i < defaultMaterial->GetPassesNum(); ++i){
                    camera->SetModelMatrix(model_mat);
                    defaultMaterial->GetPass(i)->Use(this);
                    DrawGeometryBuffer(g);
                }
            }
        }
    }

    inline void DrawEntity(MR::Entity* ent){
        if(!ent->GetModel()) return;

        float dist = MR::Transform::CalcDist( camera->GetCameraPosition(), ent->GetTransformP()->GetPos() );
        for(unsigned short i = 0; i < ent->GetModel()->GetLod( dist )->GetMeshesNum(); ++i){
            MR::Mesh* mesh = ent->GetModel()->GetLod( dist )->GetMesh(i);
            if(mesh->GetMaterialsNum() == 0) {
                for(unsigned short gi = 0; gi < mesh->GetGeomBuffersNum(); ++gi){
                    if(ent->GetMaterial() == nullptr) DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[gi], nullptr);
                    else DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[gi], ent->GetMaterial());
                }
            }
            if(mesh->GetMaterialsNum() == 1) {
                for(unsigned short gi = 0; gi < mesh->GetGeomBuffersNum(); ++gi){
                    if(ent->GetMaterial() == nullptr) DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[gi], mesh->GetMaterials()[0]);
                    else DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[gi], ent->GetMaterial());
                }
            } else {
                for(unsigned short mi = 0; mi < mesh->GetMaterialsNum(); ++mi){
                    if(ent->GetMaterial() == nullptr ) DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[mi], mesh->GetMaterials()[mi]);
                    else DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[mi], ent->GetMaterial());
                }
            }
        }
    }

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
};

}

#endif
