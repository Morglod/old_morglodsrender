#include "RenderSystem.hpp"
#include "RenderWindow.hpp"
#include "Viewport.hpp"
#include "MachineInfo.hpp"
#include "Texture.hpp"
#include "GeometryBufferV2.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Entity.hpp"
#include "Model.hpp"
#include "RenderTarget.hpp"
#include "Shader.hpp"
#include "Log.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

#ifndef _GLIBCXX_VECTOR
#   include <vector>
#endif

std::vector<MR::IRenderSystem*> renderSystems;
size_t aliveRenderSystems = 0;

void _OutOfMemory(){
    MR::Log::LogString("Out of memory", MR_LOG_LEVEL_ERROR);
    throw std::bad_alloc();
}

typedef void (*OutOfMemPtr)();
std::vector<OutOfMemPtr> outOfMemPtrs;

void OutOfMemEvent(){
    for(auto it = outOfMemPtrs.begin(); it != outOfMemPtrs.end(); ++it){
        (*it)();
    }
}

namespace MR {

void RegisterRenderSystem(IRenderSystem* rs){
    renderSystems.push_back(rs);
}

void UnRegisterRenderSystem(IRenderSystem* rs){
    std::vector<MR::IRenderSystem*>::iterator it = std::find(renderSystems.begin(), renderSystems.end(), rs);
    if(it == renderSystems.end()) return;
    renderSystems.erase(it);
}

void RenderSystemInitializated(IRenderSystem* rs){
    ++aliveRenderSystems;
}

void RenderSystemShutdowned(IRenderSystem* rs){
    --aliveRenderSystems;
}

bool AnyRenderSystemAlive(){
    return (bool)aliveRenderSystems;
}

/** ---------------------------------------------------------- */

bool RenderSystem::Init(IRenderWindow* window) {
    if(window == nullptr) return false;

    if(!_init){
        outOfMemPtrs.push_back(std::set_new_handler(OutOfMemEvent));
    }

    if(!_alive){
        MR::Log::LogString("RenderSystem initialization", MR_LOG_LEVEL_INFO);
        if(!_glfw){
            if(!glfwInit()) {
                MR::Log::LogString("glfw initialization failed", MR_LOG_LEVEL_ERROR);
                _glfw = false;
                return false;
            } else {
                MR::Log::LogString("glfw ok", MR_LOG_LEVEL_INFO);
                _glfw = true;
            }
        }
        _alive = true;
    }

    if(!window->Init()) {
        MR::Log::LogString("Window initialization failed", MR_LOG_LEVEL_ERROR);
        return false;
    }

    if(!_glew){
        GLenum result = glewInit();
        if(result != GLEW_OK) {
            MR::Log::LogString("glew initialization failed", MR_LOG_LEVEL_ERROR);
            throw std::exception();
        }
    }

    if(MR::MachineInfo::gl_version() == MR::MachineInfo::GLVersion::VNotSupported) {
        MR::Log::LogString("current opengl version (\""+MR::MachineInfo::gl_version_string()+"\") is not supported", MR_LOG_LEVEL_WARNING);
    }

    if(!_init) {
        RenderSystemInitializated(this);
        _init = true;
    }

    _window = window;

    return true;
}

void RenderSystem::Shutdown() {
    delete MR::ModelManager::Instance();
    delete MR::ShaderManager::Instance();
    delete MR::TextureManager::Instance();

    MR::Log::LogString("RenderSystem shutdown", MR_LOG_LEVEL_INFO);

    _alive = false;

    glfwTerminate();
    RenderSystemShutdowned(this);
}

void RenderSystem::SetViewport(const unsigned short & x, const unsigned short & y, const unsigned short & width, const unsigned short & height) {
    glViewport(x, y, width, height);
}

void RenderSystem::SetViewport(IViewport* vp) {
    if(_viewport != vp){
        glm::ivec4 v = vp->GetRect();
        SetViewport(v.x, v.y, v.z, v.w);
        _viewport = vp;
    }
}

void RenderSystem::SetPolygonMode(const PolygonMode& pm){
    glPolygonMode(GL_FRONT_AND_BACK, pm);
    _poly_mode = pm;
}

IRenderSystem::PolygonMode RenderSystem::GetPolygonMode(){
    return _poly_mode;
}

void RenderSystem::UseCamera(Camera* cam){
    if(_cam != cam){
        _cam = cam;
    }
}

void RenderSystem::UseShader(IShader* shader){
    _shader = shader;
    if(shader) _shader->Use(this);
}

unsigned int RenderSystem::FreeTextureUnit(){
    unsigned int u = _nextFreeUnit;
    for(size_t it = 0; 0 < _textures.size(); ++it){
        if(_textures[it].gl_texture == 0) {
            _nextFreeUnit = it;
            return u;
        }
    }
    return (unsigned int) _textures.size();
}

void RenderSystem::BindTexture(ITexture* tex, const unsigned int& unit){
    _AllocateTextureUnits(unit);

    _textures[unit].tex = tex;
    _textures[unit].settings = tex->GetSettings();
    _textures[unit].gl_texture = tex->GetGPUHandle();
    _textures[unit].isArray = (bool)(tex->GetTextureArray());

    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glBindMultiTextureEXT(GL_TEXTURE0+unit, (unsigned int)tex->GetTarget(), tex->GetGPUHandle());
    } else {
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture((unsigned int)tex->GetTarget(), tex->GetGPUHandle());
    }

    if(_textures[unit].settings == nullptr) glBindSampler(unit, 0);
    else glBindSampler(unit, _textures[unit].settings->GetGLSampler());
}

void RenderSystem::BindTexture(const unsigned int& target, const unsigned int& gpuHandle, const unsigned int& unit){
    _AllocateTextureUnits(unit);

    _textures[unit].tex = nullptr;
    _textures[unit].settings = nullptr;
    _textures[unit].gl_texture = gpuHandle;
    _textures[unit].isArray = false;

    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glBindMultiTextureEXT(GL_TEXTURE0+unit, target, gpuHandle);
    } else {
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(target, gpuHandle);
    }
}

void RenderSystem::UnBindTexture(const unsigned int& unit){
    if(unit >= _textures.size()) return;

    _textures[unit].gl_texture = 0;
    _textures[unit].tex = nullptr;
    _textures[unit].settings = nullptr;
    _textures[unit].isArray = false;
    if(_nextFreeUnit > unit) _nextFreeUnit = unit;
}

void RenderSystem::BindTextureSettings(ITextureSettings* ts, const unsigned int& unit){
    _AllocateTextureUnits(unit);

    _textures[unit].settings = ts;

    if(ts == nullptr) glBindSampler(unit, 0);
    else glBindSampler(unit, ts->GetGLSampler());
}

void RenderSystem::BindVertexFormat(IVertexFormat* format) {
    if(_vformat != format){
        if((_vformat != nullptr) && (format != nullptr)){
            if(_vformat->Equal(format)) {
                _vformat = format;
                return;
            }
        }
        if(_vformat != nullptr) _vformat->UnBind();
        if(format != nullptr){
            _vformat = format;
            _vformat->Bind();
        }
    }
}

void RenderSystem::BindIndexFormat(IIndexFormat* format) {
    if(_iformat != format){
        if(_iformat != nullptr) _iformat->UnBind();
        if(format != nullptr){
            _iformat = format;
            _iformat->Bind();
        }
    }
}

void RenderSystem::BindRenderTarget(RenderTarget* t){
    if(_renderTarget != t){
        glBindFramebuffer(GL_FRAMEBUFFER, t->GetFrameBuffer());
        SetViewport(0, 0, t->GetWidth(), t->GetHeight());
    }
}

void RenderSystem::UnBindRenderTarget(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _renderTarget = nullptr;
}

void RenderSystem::DrawGeometry(IGeometry* gb) {
    if(gb) gb->Draw(this);
}

void RenderSystem::DrawGeomWithMaterial(glm::mat4* model_mat, MR::IGeometry* g, MR::Material* mat) {
    if(mat) {
        for(unsigned short i = 0; i < mat->GetPassesNum(); ++i) {
            _cam->SetModelMatrix(model_mat);
            mat->GetPass(i)->Use(this);
            DrawGeometry(g);
        }
    } else {
        if(_useDefaultMaterial && _defaultMaterial) {
            for(unsigned short i = 0; i < _defaultMaterial->GetPassesNum(); ++i) {
                _cam->SetModelMatrix(model_mat);
                _defaultMaterial->GetPass(i)->Use(this);
                DrawGeometry(g);
            }
        } else {
            _cam->SetModelMatrix(model_mat);
            DrawGeometry(g);
        }
    }
}

void RenderSystem::DrawGeomWithMaterialPass(glm::mat4* model_mat, MR::IGeometry* g, MR::MaterialPass* mat_pass) {
    _cam->SetModelMatrix(model_mat);
    if(mat_pass) mat_pass->Use(this);
    DrawGeometry(g);
}

void RenderSystem::DrawEntity(MR::Entity* ent) {
    if(!ent->GetModel()) return;
    float dist = MR::Transform::CalcDist( *_cam->GetPosition(), ent->GetTransformP()->GetPos() );
    ModelLod* lod = ent->GetModel()->GetLod( dist );
    if(!lod) return;
    for(unsigned short i = 0; i < lod->GetMeshesNum(); ++i) {
        MR::Mesh* mesh = lod->GetMesh(i);
        if(mesh->GetMaterial() == nullptr) {
            for(unsigned short gi = 0; gi < mesh->GetGeomNum(); ++gi) {
                if(ent->GetMaterial() == nullptr) DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeoms()[gi], nullptr);
                else DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeoms()[gi], ent->GetMaterial());
            }
        }
        else {
            for(unsigned short gi = 0; gi < mesh->GetGeomNum(); ++gi) {
                if(ent->GetMaterial() == nullptr) DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeoms()[gi], mesh->GetMaterial());
                else DrawGeomWithMaterial(ent->GetTransformP()->GetMatP(), mesh->GetGeoms()[gi], ent->GetMaterial());
            }
        }
    }
}


RenderSystem::RenderSystem() : MR::Object(), _init(false), _alive(false), _glew(false), _glfw(false),
    _viewport(nullptr), _nextFreeUnit(0), _vformat(nullptr), _iformat(nullptr), _cam(nullptr), _shader(nullptr), _useDefaultMaterial(true), _defaultMaterial(nullptr), _renderTarget(nullptr), _poly_mode((IRenderSystem::PolygonMode)GL_FILL), _window(nullptr) {

    RegisterRenderSystem(this);
}

RenderSystem::~RenderSystem() {
    if(_alive){
        Shutdown();
    }
    UnRegisterRenderSystem(this);
}

}
