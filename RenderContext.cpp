#include "RenderContext.hpp"


#include "Log.hpp"
#include "Camera.hpp"
#include "GeometryBuffer.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "Entity.hpp"

namespace MR {

void RenderContext::UseGeometryBuffer(GeometryBuffer* gb) {
    if(gb != _gb) {
        _gb = gb;
        _gb->Bind();
    }
}

void RenderContext::UseShader(Shader* sh) {
    _sh = sh;
    if(_sh != nullptr) _sh->Use();
}

void RenderContext::ActiveTextureUnit(const GLenum & u) {
    if(activedTextureUnit != u) {
        glActiveTexture(u);
        activedTextureUnit = u;
    }
}

void RenderContext::BindTexture(const GLuint& tx, GLenum texStage) {
    ActiveTextureUnit(texStage);
    texStage -= GL_TEXTURE0;
    if( _tx[texStage] != tx ) {
        _tx[texStage] = tx;
        glBindTexture(GL_TEXTURE_2D, tx);
    }
}

void RenderContext::BindTexture(Texture* tx, const GLenum& texStage) {
    BindTexture(tx->GetGLTexture(), texStage);
    UseTextureSettings(tx->GetSettings(), texStage);
}

void RenderContext::UseTextureSettings(TextureSettings* ts, GLenum texStage) {
    texStage -= GL_TEXTURE0;
    if( _ts[texStage] != ts ) {
        _ts[texStage] = ts;
        if(ts == nullptr) glBindSampler(texStage, 0);
        else glBindSampler(texStage, ts->GetGLSampler());
    }
}

void RenderContext::SetDefaultMaterial(Material* mat) {
    if(defaultMaterial != mat) {
        defaultMaterial = mat;
        OnDefaultMaterialChanged(this, mat);
    }
}

void RenderContext::SetViewport(const unsigned short & x, const unsigned short & y, const unsigned short & width, const unsigned short & height) {
    if( (vp_x != x) || (vp_y != y) || (vp_w != width) || (vp_h != height) ) {
        glViewport(x, y, width, height);
        vp_x = x;
        vp_y = y;
        vp_w = width;
        vp_h = height;
    }
}

void RenderContext::UseDefaultMaterialState(bool s) {
    if(useDefaultMaterial != s) {
        useDefaultMaterial = s;
        OnDefaultMaterialUsingStateChanged(this, s);
    }
}

void RenderContext::DrawGeometryBuffer(GeometryBuffer* gb) {
    UseGeometryBuffer(gb);
    _gb->Draw();
}

void RenderContext::DrawGeomWithMaterial(glm::mat4* model_mat, MR::GeometryBuffer* g, MR::Material* mat) {
    //UseGeometryBuffer(g);
    if(mat) {
        for(unsigned short i = 0; i < mat->GetPassesNum(); ++i) {
            camera->SetModelMatrix(model_mat);
            mat->GetPass(i)->Use(this);
            DrawGeometryBuffer(g);
        }
    } else {
        if(useDefaultMaterial && defaultMaterial) {
            for(unsigned short i = 0; i < defaultMaterial->GetPassesNum(); ++i) {
                camera->SetModelMatrix(model_mat);
                defaultMaterial->GetPass(i)->Use(this);
                DrawGeometryBuffer(g);
            }
        }
    }
}

void RenderContext::DrawEntity(MR::Entity* ent) {
    if(!ent->GetModel()) return;

    float dist = MR::Transform::CalcDist( camera->GetCameraPosition(), ent->GetTransformP()->GetPos() );
    for(unsigned short i = 0; i < ent->GetModel()->GetLod( dist )->GetMeshesNum(); ++i) {
        MR::Mesh* mesh = ent->GetModel()->GetLod( dist )->GetMesh(i);
        if(mesh->GetMaterialsNum() == 0) {
            for(unsigned short gi = 0; gi < mesh->GetGeomBuffersNum(); ++gi) {
                if(ent->GetMaterial() == nullptr) DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[gi], nullptr);
                else DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[gi], ent->GetMaterial());
            }
        }
        if(mesh->GetMaterialsNum() == 1) {
            for(unsigned short gi = 0; gi < mesh->GetGeomBuffersNum(); ++gi) {
                if(ent->GetMaterial() == nullptr) DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[gi], mesh->GetMaterials()[0]);
                else DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[gi], ent->GetMaterial());
            }
        } else {
            for(unsigned short mi = 0; mi < mesh->GetMaterialsNum(); ++mi) {
                if(ent->GetMaterial() == nullptr ) DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[mi], mesh->GetMaterials()[mi]);
                else DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[mi], ent->GetMaterial());
            }
        }
    }
}

bool RenderContext::Init() {
    MR::Log::LogString("MorglodsRender context initialization", MR_LOG_LEVEL_INFO);
    if(!glfwInit()) {
        MR::Log::LogString("glfw initialization failed", MR_LOG_LEVEL_ERROR);
        return false;
    } else {
        MR::Log::LogString("glfw ok", MR_LOG_LEVEL_INFO);
    }

    return true;
}

RenderContext::RenderContext() : camera(nullptr) {
}

RenderContext::~RenderContext() {
    MR::Log::LogString("MorglodsRender shutdown", MR_LOG_LEVEL_INFO);
    MR::TextureManager::Instance()->RemoveAll();
    delete MR::TextureManager::Instance();
    MR::ModelManager::Instance()->RemoveAll();
    delete MR::ModelManager::Instance();
    MR::ShaderManager::Instance()->RemoveAll();
    delete MR::ShaderManager::Instance();
    glfwTerminate();
}

}
