#include "Core.hpp"
#include "MachineInfo.hpp"
#include "Geometry/GeometryBuffer.hpp"
#include "Geometry/GeometryFormats.hpp"
#include "Materials/MaterialInterfaces.hpp"
#include "Geometry/GeometryManager.hpp"
#include "Textures/TextureObjects.hpp"
#include "Buffers/Buffers.hpp"
#include "Shaders/ShaderObjects.hpp"
#include "Utils/FilesIO.hpp"
#include "Context.hpp"
#include "Utils/Log.hpp"

#include <GL/glew.h>

PFNGLBUFFERSTORAGEPROC __glewBufferStorage;
PFNGLNAMEDBUFFERSTORAGEEXTPROC __glewNamedBufferStorageEXT;

bool MR::Init(MR::IContext* ctx) {
    if(!ctx) {
        MR::Log::LogString("Failed Init(). ctx is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    MR::IContext::Current = ctx;

    {   //glew
        GLenum result = glewInit();
        if(result != GLEW_OK) {
            MR::Log::LogString("Failed Init(). glew initialization failed. "+std::string((char*)glewGetErrorString(result)), MR_LOG_LEVEL_ERROR);
            return false;
        }

        __glewBufferStorage = (PFNGLBUFFERSTORAGEPROC)(ctx->GetProcAddress("glBufferStorage"));
        __glewNamedBufferStorageEXT = (PFNGLNAMEDBUFFERSTORAGEEXTPROC)(ctx->GetProcAddress("glNamedBufferStorageEXT"));
    }

    if(MR::MachineInfo::gl_version() == MR::MachineInfo::GLVersion::VNotSupported) {
        MR::Log::LogString("Current opengl version (\""+MR::MachineInfo::gl_version_string()+"\") is not supported. OpenGL 3.2 will be used.", MR_LOG_LEVEL_WARNING);
    }

    return true;
}

void MR::Shutdown() {
    MR::DestroyAllTextures();
    MR::DestroyAllBuffers();
    MR::DestroyAllShaderPrograms();
    MR::GeometryManager::DestroyInstance();
    MR::FileUtils::DestroyInstance();
}
