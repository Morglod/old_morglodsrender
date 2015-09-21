#include "../mr/Core.hpp"
//#include "../mr/MachineInfo.hpp"

#include "../mr/pre/glew.hpp"
#include "../mr/Log.hpp"
#include "../mr/MachineInfo.hpp"

namespace mr {

bool Core::Init() {
    glewExperimental = true;
    GLenum result = glewInit();
    if(result != GLEW_OK) {
        MR_LOG_ERROR(Core::Init, "glew initialization failed "+std::string((char*)glewGetErrorString(result)));
        return false;
    }

    if(Info::GetVersion() == Info::GLVersion::VNotSupported) {
        MR_LOG_WARNING(Core::Init, "Current opengl version (\""+Info::GetVersionAsString()+"\") is not supported. Try to use OpenGL 4.0 features");
    }

    return true;
}

void Core::Shutdown() {
}

}
