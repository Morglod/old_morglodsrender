#include "Core.hpp"
#include "MachineInfo.hpp"
#include "Utils/Log.hpp"

#include "Geometry/GeometryManager.hpp"
#include "Utils/FilesIO.hpp"
#include "Buffers/BuffersManager.hpp"

#include <GL/glew.h>

PFNGLBUFFERSTORAGEPROC __glewBufferStorage;
PFNGLNAMEDBUFFERSTORAGEEXTPROC __glewNamedBufferStorageEXT;

namespace mr {

void DestroyAllTextures();
void DestroyAllShaderPrograms();

}

bool mr::Init() {
    GLenum result = glewInit();
    if(result != GLEW_OK) {
        mr::Log::LogString("Failed Init(). glew initialization failed. "+std::string((char*)glewGetErrorString(result)), MR_LOG_LEVEL_ERROR);
        return false;
    }

    if(mr::gl::GetVersion() == mr::gl::GLVersion::VNotSupported) {
        mr::Log::LogString("Current opengl version (\""+mr::gl::GetVersionAsString()+"\") is not supported. Try to use OpenGL 4.0 features.", MR_LOG_LEVEL_WARNING);
    }

    return true;
}

void mr::Shutdown() {
    mr::DestroyAllTextures();
    mr::GPUBuffersManager::GetInstance().DestroyAllBuffers();
    mr::DestroyAllShaderPrograms();
    mr::GeometryManager::DestroyInstance();
}
