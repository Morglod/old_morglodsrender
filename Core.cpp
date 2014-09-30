#include "Core.hpp"
#include "MachineInfo.hpp"
#include "Geometry/GeometryBuffer.hpp"
#include "Geometry/GeometryFormats.hpp"
#include "Materials/MaterialInterfaces.hpp"
#include "Geometry/GeometryManager.hpp"
//#include "Model.hpp"
#include "Utils/FilesIO.hpp"
#include "Context.hpp"
#include "Utils/Log.hpp"

#include <GL/glew.h>

void _OutOfMemory(){
    MR::Log::LogString("Out of memory", MR_LOG_LEVEL_ERROR);
    throw std::bad_alloc();
}

typedef void (*OutOfMemPtr)();
std::vector<OutOfMemPtr> outOfMemPtrs;

void OutOfMemEvent(){
    for(size_t i = 0; i < outOfMemPtrs.size(); ++i){
        outOfMemPtrs[i]();
    }
}

PFNGLBUFFERSTORAGEPROC __glewBufferStorage;
PFNGLNAMEDBUFFERSTORAGEEXTPROC __glewNamedBufferStorageEXT;

bool MR::Init(MR::IContext* ctx) {
    if(!ctx) {
        MR::Log::LogString("Failed Init(). ctx is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    outOfMemPtrs.push_back(std::set_new_handler(OutOfMemEvent));

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
    //TODO MR::RenderManager::DestroyInstance();

    /*MR::VertexDataTypeFloat::DestroyInstance();
    MR::VertexDataTypeInt::DestroyInstance();
    MR::VertexDataTypeUInt::DestroyInstance();
    MR::VertexAttributePos3F::DestroyInstance();*/
    MR::GeometryManager::DestroyInstance();

    //TODO MR::TextureManager::DestroyInstance();

    //TODO MR::MaterialManager::DestroyInstance();

    //MR::ModelManager::DestroyInstance();

    MR::FileUtils::DestroyInstance();
}
