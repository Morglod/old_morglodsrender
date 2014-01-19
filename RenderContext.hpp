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
    Texture* _tx[GL_TEXTURE31-GL_TEXTURE0+1];
    Texture* _last_tx = nullptr;

    Material* defaultMaterial = nullptr;
    bool useDefaultMaterial = true;

public:
    MR::Event<Material*> OnDefaultMaterialChanged;
    MR::Event<bool> OnDefaultMaterialUsingStateChanged;

    Camera* camera;

    inline GeometryBuffer* GetCurrentGeometryBuffer() {
        return _gb;
    }

    inline void UseGeometryBuffer(GeometryBuffer* gb) {
        if(gb != _gb) {
            _gb = gb;
            _gb->Bind();
        }
    }

    inline Shader* GetCurrentShader() {
        return _sh;
    }

    inline void UseShader(Shader* sh) {
        _sh = sh;
        _sh->Use();
    }

    inline Texture* GetCurrentTexture(GLenum stage) {
        return _tx[stage-GL_TEXTURE0];
    }

    inline Texture** GetCurrentTextures() {
        return &_tx[0];
    }

    inline void UseTexture(Texture* tx, GLenum texStage) {
        if( _tx[texStage-GL_TEXTURE0] != tx ) {
            _tx[texStage-GL_TEXTURE0] = tx;
            tx->Bind(texStage);
        } else if( _last_tx != tx ){
            tx->Bind(texStage);
        }
        _last_tx = tx;
    }

    inline void SetDefaultMaterial(Material* mat) {
        if(defaultMaterial != mat){
            defaultMaterial = mat;
            OnDefaultMaterialChanged(this, mat);
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

    inline void DrawGeomWithMaterial(glm::mat4& model_mat, MR::GeometryBuffer* g, MR::Material* mat){
        //UseGeometryBuffer(g);
        if(mat){
            for(unsigned short i = 0; i < mat->materialPasses.size(); ++i){
                camera->SetModelMatrix(&model_mat);
                mat->materialPasses[i]->Use(this);
                DrawGeometryBuffer(g);
            }
        } else {
            if(useDefaultMaterial && defaultMaterial){
                for(unsigned short i = 0; i < defaultMaterial->materialPasses.size(); ++i){
                    camera->SetModelMatrix(&model_mat);
                    defaultMaterial->materialPasses[i]->Use(this);
                    DrawGeometryBuffer(g);
                }
            }
        }
    }

    inline void DrawEntity(MR::Entity* ent){
        if(!ent->GetModel()) return;

        for(unsigned short i = 0; i < ent->GetModel()->GetMeshesNum(); ++i){
            MR::Mesh* mesh = ent->GetModel()->GetMesh(i);
            if(mesh->GetMaterialsNum() == 0) {
                for(unsigned short gi = 0; gi < mesh->GetGeomBuffersNum(); ++gi){
                    if(ent->GetMaterial() == nullptr) DrawGeomWithMaterial(ent->GetTransform()->getMat(), mesh->GetGeomBuffers()[gi], nullptr);
                    else DrawGeomWithMaterial(ent->GetTransform()->getMat(), mesh->GetGeomBuffers()[gi], ent->GetMaterial());
                }
            }
            if(mesh->GetMaterialsNum() == 1) {
                for(unsigned short gi = 0; gi < mesh->GetGeomBuffersNum(); ++gi){
                    if(ent->GetMaterial() == nullptr) DrawGeomWithMaterial(ent->GetTransform()->getMat(), mesh->GetGeomBuffers()[gi], mesh->GetMaterials()[0]);
                    else DrawGeomWithMaterial(ent->GetTransform()->getMat(), mesh->GetGeomBuffers()[gi], ent->GetMaterial());
                }
            } else {
                for(unsigned short mi = 0; mi < mesh->GetMaterialsNum(); ++mi){
                    if(ent->GetMaterial() == nullptr ) DrawGeomWithMaterial(ent->GetTransform()->getMat(), mesh->GetGeomBuffers()[mi], mesh->GetMaterials()[mi]);
                    else DrawGeomWithMaterial(ent->GetTransform()->getMat(), mesh->GetGeomBuffers()[mi], ent->GetMaterial());
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
