#include "Core.hpp"
#include "MachineInfo.hpp"
#include "Utils/Log.hpp"

#include "Geometry/GeometryManager.hpp"
#include "Utils/FilesIO.hpp"
#include "Buffers/BuffersManager.hpp"

#include <GL/glew.h>

namespace mr {

void DestroyAllTextures();
void DestroyAllShaderPrograms();

bool Init() {
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

void Shutdown() {
    DestroyAllTextures();
    GPUBuffersManager::GetInstance().DestroyAllBuffers();
    DestroyAllShaderPrograms();
    GeometryManager::DestroyInstance();
}

}
