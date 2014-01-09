#include "RenderContext.hpp"
#include "Mesh.hpp"
#include "Log.hpp"

namespace MR {

bool RenderContext::Init() {
    MR::Log::LogString("MorglodsRender context initialization", MR_LOG_LEVEL_INFO);
    if(!glfwInit()) {
        MR::Log::LogString("glfw initialization failed", MR_LOG_LEVEL_ERROR);
        return false;
    } else {
        MR::Log::LogString("glfw ok", MR_LOG_LEVEL_INFO);
    }

    return true;
}

RenderContext::RenderContext() {
}

RenderContext::~RenderContext() {
    MR::Log::LogString("MorglodsRender shutdown", MR_LOG_LEVEL_INFO);
    MR::TextureManager::Instance()->RemoveAll();
    MR::MeshManager::Instance()->RemoveAll();
    glfwTerminate();
}

}
