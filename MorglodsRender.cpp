#include "MorglodsRender.hpp"

bool MR::Render::Init(std::string window_name, int width, int height){
    MR::Log::LogString("MorglodsRender \""+window_name+"\" "+std::to_string(width)+" "+std::to_string(height), MR_LOG_LEVEL_INFO);

    if(!glfwInit()){
        MR::Log::LogString("glfw initialization failed", MR_LOG_LEVEL_ERROR);
        return false;
    } else {
        MR::Log::LogString("glfw ok", MR_LOG_LEVEL_INFO);
    }

    window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);

    if(!window){
        MR::Log::LogString("glfw window creation failed", MR_LOG_LEVEL_ERROR);
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    GLenum result = glewInit();

    if(result != GLEW_OK){
        MR::Log::LogString("glew initialization failed", MR_LOG_LEVEL_ERROR);
        return false;
    }

    if(MR::MachineInfo::gl_version() == MR::MachineInfo::GLVersion::VNotSupported){
        MR::Log::LogString("current opengl version (\""+MR::MachineInfo::gl_version_string()+"\") is not supported", MR_LOG_LEVEL_WARNING);
    }

    glEnable(GL_TEXTURE_2D);

    released = true;
    return true;
}

void MR::Render::Shutdown(){
    if(!released){
        MR::Log::LogString("MorglodsRender shutdown", MR_LOG_LEVEL_INFO);
        MR::TextureManager::Instance()->RemoveAll();
        //MR::MeshManager::Instance()->RemoveAll();
        glfwTerminate();
        released = true;
    }
}

MR::Render::Render() : window(NULL), released(true) {
}

MR::Render::~Render(){
    Shutdown();
}
