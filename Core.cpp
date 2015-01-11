#include "Core.hpp"
#include "MachineInfo.hpp"
#include "ContextManager.hpp"
#include "Utils/Log.hpp"

#include "Geometry/GeometryManager.hpp"
#include "Utils/FilesIO.hpp"

#include <GL/glew.h>

PFNGLBUFFERSTORAGEPROC __glewBufferStorage;
PFNGLNAMEDBUFFERSTORAGEEXTPROC __glewNamedBufferStorageEXT;

namespace mr {

void DestroyAllTextures();
void DestroyAllBuffers();
void DestroyAllShaderPrograms();
void _MR_InitTextures();

}

void _MR_OneTimeInit() {
    static bool b = false;
    if(b) return;
    b = true;
    __glewBufferStorage = (PFNGLBUFFERSTORAGEPROC)(mr::IContextManager::GetCurrent()->GetProcAddress("glBufferStorage"));
    __glewNamedBufferStorageEXT = (PFNGLNAMEDBUFFERSTORAGEEXTPROC)(mr::IContextManager::GetCurrent()->GetProcAddress("glNamedBufferStorageEXT"));
    mr::_MR_InitTextures();
}

bool mr::Init(mr::IContext* ctx) {
    if(!ctx) {
        mr::Log::LogString("Failed Init() context. ctx is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    {
        GLenum result = glewInit();
        if(result != GLEW_OK) {
            mr::Log::LogString("Failed Init(). glew initialization failed. "+std::string((char*)glewGetErrorString(result)), MR_LOG_LEVEL_ERROR);
            return false;
        }
    }

    if(mr::gl::GetVersion() == mr::gl::GLVersion::VNotSupported) {
        mr::Log::LogString("Current opengl version (\""+mr::gl::GetVersionAsString()+"\") is not supported. OpenGL 4.0 will be used.", MR_LOG_LEVEL_WARNING);
    }

    _MR_OneTimeInit();

    return true;
}

bool mr::Init(mr::IContextManager* ctxMgr) {
    if(!ctxMgr) {
        mr::Log::LogString("Failed Init() context manager. ctxMgr is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    mr::IContextManager::SetCurrent(ctxMgr);

    for(size_t i = 0; i < ctxMgr->GetContextsNum(); ++i) {
        if(! mr::Init(ctxMgr->GetContext(i)) ) {
            mr::Log::LogString("Failed Init() conext manager. Failed init context " + std::to_string(i), MR_LOG_LEVEL_ERROR);
            return false;
        }
    }

    return true;
}

void mr::Shutdown() {
    mr::DestroyAllTextures();
    mr::DestroyAllBuffers();
    mr::DestroyAllShaderPrograms();
    mr::GeometryManager::DestroyInstance();
}
