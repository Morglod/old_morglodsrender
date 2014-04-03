#include "RenderContext.hpp"

#include "Log.hpp"
#include "Camera.hpp"
#include "GeometryBufferV2.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "Entity.hpp"
#include "UI.hpp"
#include "MachineInfo.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

#include <vector>

namespace MR {

RenderContext* RenderContext::_instance;

void RenderContext::UseCamera(Camera* cam){
    if(cam != _cam){
        _cam = cam;
        OnCameraUsed(this, _cam);
    }
}

/*void RenderContext::UseGeometryBuffer(GeometryBuffer* gb) {
    if(gb != _gb) {
        _gb = gb;
        _gb->Bind();
        OnGeometryBufferUsed(this, _gb);
    }
}*/

void RenderContext::UseShader(Shader* sh) {
    _sh = sh;
    if(_sh != nullptr) {
        if(sh->Use(this)) OnShaderUsed(this, _sh);
    }
}

void RenderContext::ActiveTextureUnit(const GLenum & u) {
    if(activedTextureUnit != u) {
        glActiveTexture(GL_TEXTURE0+u);
        activedTextureUnit = u;
        OnTextureUnitActived(this, u);
    }
}

void RenderContext::BindTexture(const Texture::Target& target, const GLuint& tx, const unsigned int& texStage) {
    if( _tx[texStage] != tx ) {
        _tx[texStage] = tx;
        _tab[texStage] = false;

        if(MR::MachineInfo::IsDirectStateAccessSupported()){
            glBindMultiTextureEXT(GL_TEXTURE0+texStage, (unsigned int)target, tx);
        } else {
            ActiveTextureUnit(texStage);
            glBindTexture((unsigned int)target, tx);
        }
    }
}

void RenderContext::BindTexture(Texture* tx, const unsigned int& texStage) {
    if(tx->_inTextureArray) {
        //MR::Log::LogString("Bind tex array; Index = "+std::to_string(tx->_textureArrayIndex)+"; GLTexture = "+std::to_string(tx->_texArray->GetGLTexture())+"; stage = "+std::to_string( (int)(texStage - GL_TEXTURE0)));
        if( _tx[texStage] != tx->GetGLTexture() ){
            _tab[texStage] = true;
            _tai[texStage] = tx->_textureArrayIndex;
            _tx[texStage] = tx->_texArray->GetGLTexture();

            if(MR::MachineInfo::IsDirectStateAccessSupported()){
                glBindMultiTextureEXT(GL_TEXTURE0+texStage, (unsigned int)tx->_target, _tx[texStage]);
            } else {
                ActiveTextureUnit(texStage);
                glBindTexture(GL_TEXTURE_2D_ARRAY, _tx[texStage]);
                UseTextureSettings(tx->GetSettings(), texStage);
            }
        }
    } else {
        BindTexture(tx->_target, tx->GetGLTexture(), texStage);
        UseTextureSettings(tx->GetSettings(), texStage);
    }
}

void RenderContext::BindVertexFormat(IVertexFormat* format){
    if(_vformat != format){
        if(_vformat != nullptr) _vformat->UnBind();
        if(format != nullptr){
            _vformat = format;
            _vformat->Bind();
        }
    }
}

void RenderContext::BindIndexFormat(IIndexFormat* format){
    if(_iformat != format){
        if(_iformat != nullptr) _iformat->UnBind();
        if(format != nullptr){
            _iformat = format;
            _iformat->Bind();
        }
    }
}

void RenderContext::UseTextureSettings(TextureSettings::Ptr ts, const unsigned int& texStage) {
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

void RenderContext::UseDefaultMaterial(const bool& s) {
    if(useDefaultMaterial != s) {
        useDefaultMaterial = s;
        OnDefaultMaterialUsingStateChanged(this, s);
    }
}

void RenderContext::DrawGeometryBuffer(GeometryBuffer* gb) {
    //UseGeometryBuffer(gb);
    gb->Draw(this);
}

void RenderContext::DrawGeomWithMaterialPass(glm::mat4* model_mat, MR::GeometryBuffer* g, MR::MaterialPass* mat_pass) {
    if(mat_pass) {
        _cam->SetModelMatrix(model_mat);
        mat_pass->Use(this);
        DrawGeometryBuffer(g);
    } else {
        _cam->SetModelMatrix(model_mat);
        DrawGeometryBuffer(g);
    }
}

void RenderContext::DrawGeomWithMaterial(glm::mat4* model_mat, MR::GeometryBuffer* g, MR::Material* mat) {
    //UseGeometryBuffer(g);
    if(mat) {
        for(unsigned short i = 0; i < mat->GetPassesNum(); ++i) {
            _cam->SetModelMatrix(model_mat);
            mat->GetPass(i)->Use(this);
            DrawGeometryBuffer(g);
        }
    } else {
        if(useDefaultMaterial && defaultMaterial) {
            for(unsigned short i = 0; i < defaultMaterial->GetPassesNum(); ++i) {
                _cam->SetModelMatrix(model_mat);
                defaultMaterial->GetPass(i)->Use(this);
                DrawGeometryBuffer(g);
            }
        } else {
            _cam->SetModelMatrix(model_mat);
            DrawGeometryBuffer(g);
        }
    }
}

void RenderContext::DrawEntity(MR::Entity* ent) {
    if(!ent->GetModel()) return;
    float dist = MR::Transform::CalcDist( *_cam->GetPosition(), ent->GetTransformP()->GetPos() );
    ModelLod* lod = ent->GetModel()->GetLod( dist );
    for(unsigned short i = 0; i < lod->GetMeshesNum(); ++i) {
        MR::Mesh* mesh = lod->GetMesh(i);
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
                else {
                        DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeomBuffers()[mi], ent->GetMaterial());
                }
            }
        }
    }
}

struct _DrawParam {
public:
    std::vector<glm::mat4*> matr; ///MAY BE PROBLEMS THAT IT IS VECTOR IN VECTOR
    GeometryBuffer* buffer;
    MaterialPass* mat;
    unsigned int num;

    _DrawParam(MR::Entity* _e, GeometryBuffer* _b, MaterialPass* _mp) : matr(), buffer(_b), mat(_mp), num(1) {
        matr.push_back(_e->GetTransformP()->GetMatP());
    }
};

inline void _IncSceneGeom(std::vector<_DrawParam>& sg, MR::Entity* e, GeometryBuffer* b, MaterialPass* mp){
    for(auto it = sg.begin(); it != sg.end(); ++it){
        if( (it->buffer == b) && (it->mat == mp) ) {
            it->matr.push_back(e->GetTransformP()->GetMatP());
            it->num++;
            return;
        }
    }
    sg.push_back(_DrawParam(e, b, mp));
}

void RenderContext::DrawEntity(MR::Entity** ent_list, const unsigned int& num, const bool& instancing){
    if(instancing)
    {
        //First analize entities
        std::vector<_DrawParam> scene_geometry;

        for(unsigned int it = 0; it < num; ++it){
            if(!ent_list[it]->GetModel()) continue;

            float dist = MR::Transform::CalcDist( *_cam->GetPosition(), ent_list[it]->GetTransformP()->GetPos() );
            MR::ModelLod* lod = ent_list[it]->GetModel()->GetLod(dist);
            for(unsigned short it_lod_mesh = 0; it_lod_mesh < lod->GetMeshesNum(); ++it_lod_mesh){
                for(unsigned int it_lod_mesh_geom = 0; it_lod_mesh_geom < lod->GetMesh(it_lod_mesh)->GetGeomBuffersNum(); ++it_lod_mesh_geom){
                    if(lod->GetMesh(it_lod_mesh)->GetMaterialsNum() == 0) {
                        _IncSceneGeom(scene_geometry, ent_list[it], lod->GetMesh(it_lod_mesh)->GetGeomBuffers()[it_lod_mesh_geom], nullptr);
                    } else if(lod->GetMesh(it_lod_mesh)->GetMaterialsNum() == 1) {
                        for(unsigned short mpass = 0; mpass < lod->GetMesh(it_lod_mesh)->GetMaterials()[0]->GetPassesNum(); ++mpass) {
                            _IncSceneGeom(scene_geometry, ent_list[it], lod->GetMesh(it_lod_mesh)->GetGeomBuffers()[it_lod_mesh_geom], lod->GetMesh(it_lod_mesh)->GetMaterials()[0]->GetPass(mpass) );
                        }
                    } else {
                        for(unsigned short mats_it = 0; mats_it < lod->GetMesh(it_lod_mesh)->GetMaterialsNum(); ++mats_it) {
                            for(unsigned short mpass = 0; mpass < lod->GetMesh(it_lod_mesh)->GetMaterials()[0]->GetPassesNum(); ++mpass) {
                                _IncSceneGeom(scene_geometry, ent_list[it], lod->GetMesh(it_lod_mesh)->GetGeomBuffers()[it_lod_mesh_geom], lod->GetMesh(it_lod_mesh)->GetMaterials()[mats_it]->GetPass(mpass) );
                            }
                        }
                    }
                }
            }
        }

        std::vector<glm::mat4> mat_buffer_m;
        for(auto it = scene_geometry.begin(); it != scene_geometry.end(); ++it){
            for(auto mit = it->matr.begin(); mit != it->matr.end(); ++mit){
                mat_buffer_m.push_back( *(*mit) );
            }
        }

        static GLuint mat_buffer = 0;
        if(mat_buffer == 0){
            glGenBuffers(1, &mat_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, mat_buffer);
            glBufferData(GL_ARRAY_BUFFER, mat_buffer_m.size()*sizeof(glm::mat4), &mat_buffer_m[0], GL_STREAM_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        //Draw it
        for(auto it = scene_geometry.begin(); it != scene_geometry.end(); ++it){
            for(auto mit = it->matr.begin(); mit != it->matr.end(); ++mit){
                DrawGeomWithMaterialPass((*mit), it->buffer, it->mat);
            }
        }
    } else {
        for(unsigned int it = 0; it < num; ++it){
            DrawEntity(ent_list[it]);
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
    _is_alive = true;

    return true;
}

RenderContext::RenderContext() : _is_alive(false) {
    RenderContext::_instance = this;
}

RenderContext::~RenderContext() {
    _is_alive = false;
    RenderContext::_instance = nullptr;

    MR::Log::LogString("MorglodsRender shutdown", MR_LOG_LEVEL_INFO);
    delete MR::UIManager::Instance();
    MR::TextureManager::Instance()->RemoveAll();
    delete MR::TextureManager::Instance();
    MR::ModelManager::Instance()->RemoveAll();
    delete MR::ModelManager::Instance();
    MR::ShaderManager::Instance()->RemoveAll();
    delete MR::ShaderManager::Instance();
    delete MR::UIManager::Instance();
    glfwTerminate();
}

unsigned short MR::RenderContext::FPS() {
    static float lastTime = 0.0;
    static float time = 0.0;
    static unsigned short frames = 0;
    static unsigned short lastFps = 0;

    float currentTime = (float)glfwGetTime();
    time += currentTime-lastTime;
    lastTime = currentTime;
    ++frames;
    if(time > 1) {
        lastFps = frames;
        time -= 1.0;
        frames = 0;
    }

    return lastFps;
}

float MR::RenderContext::Delta() {
    static float lastTime = 0;
    float cur = (float)glfwGetTime();
    float d = cur-lastTime;
    lastTime = cur;
    return d;
}

}
