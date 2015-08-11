#include "Core.hpp"
#include "MachineInfo.hpp"
#include "Utils/Log.hpp"

#include "Geometry/GeometryManager.hpp"
#include "Utils/FilesIO.hpp"
#include "Buffers/BufferManager.hpp"
#include "Textures/TextureManager.hpp"

#include <GL/glew.h>

namespace mr {

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
    TextureManager::GetInstance().DestroyAllTextures();
    GeometryManager::DestroyInstance(); //here gpuBuffers is used, so destroy it before gpuBuffers.
    BufferManager::GetInstance().DestroyAll();
}

}
