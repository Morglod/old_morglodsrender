#include "mr/core.hpp"

#include "mr/pre/glew.hpp"
#include "mr/log.hpp"
#include "mr/info.hpp"
#include "src/statecache.hpp"

#include "src/mp.hpp"

namespace {

thread_local bool core_thread = false;

}

namespace mr {

bool Core::Init() {
    if(!mp::Init()) {
        MR_LOG_WARNING(Core::Init, "Failed init profiler");
    }

    MP_ScopeSample(Core::Init);

    core_thread = true;

    glewExperimental = true;
    GLenum result = glewInit();
    if(result != GLEW_OK) {
        MR_LOG_ERROR(Core::Init, "glew initialization failed "+std::string((char*)glewGetErrorString(result)));
        MP_EndSample();
        return false;
    }

    if(Info::GetVersion() == Info::GLVersion::VNotSupported) {
        MR_LOG_WARNING(Core::Init, "Current opengl version (\""+Info::GetVersionAsString()+"\") is not supported. Try to use OpenGL 4.0 features");
    }

    if(!StateCache::Get()->_Init()) {
        MR_LOG_ERROR(Core::Init, "Failed init StateCache");
        MP_EndSample();
        return false;
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint workingVA;
    glGenVertexArrays(1, &workingVA);
    glBindVertexArray(workingVA);

    if(GLEW_NV_vertex_buffer_unified_memory) {
        MR_LOG("NV_vertex_buffer_unified_memory is used");
        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    }

    return true;
}

void Core::Shutdown() {
    MP_BeginSample(Core::Shutdown);

    MP_EndSample();
    mp::Shutdown();
}

bool Core::IsCoreThread() {
    return core_thread;
}

}
