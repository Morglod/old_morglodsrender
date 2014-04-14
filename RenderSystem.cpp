#include "RenderSystem.hpp"
#include "Log.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

#ifndef _GLIBCXX_VECTOR
#   include <vector>
#endif

std::vector<MR::IRenderSystem*> renderSystems;
size_t aliveRenderSystems = 0;

namespace MR {

void RegisterRenderSystem(IRenderSystem* rs){
    renderSystems.push_back(rs);
}

void UnRegisterRenderSystem(IRenderSystem* rs){
    std::vector<MR::IRenderSystem*>::iterator it = std::find(renderSystems.begin(), renderSystems.end(), rs);
    if(it == renderSystems.end()) return;
    renderSystems.erase(it);
}

void RenderSystemInitializated(IRenderSystem* rs){
    ++aliveRenderSystems;
}

void RenderSystemShutdowned(IRenderSystem* rs){
    --aliveRenderSystems;
}

bool AnyRenderSystemAlive(){
    return (bool)aliveRenderSystems;
}

bool RenderSystem::Init() {
    MR::Log::LogString("RenderSystem initialization", MR_LOG_LEVEL_INFO);
    if(!glfwInit()) {
        MR::Log::LogString("glfw initialization failed", MR_LOG_LEVEL_ERROR);
        return false;
    } else {
        MR::Log::LogString("glfw ok", MR_LOG_LEVEL_INFO);
    }
    _alive = true;

    RenderSystemInitializated(this);
    return true;
}

void RenderSystem::Shutdown() {
    MR::Log::LogString("RenderSystem shutdown", MR_LOG_LEVEL_INFO);

    _alive = false;

    glfwTerminate();
    RenderSystemShutdowned(this);
}

RenderSystem::RenderSystem() : MR::Object(), _alive(false), _glew(false) {
    RegisterRenderSystem(this);
}

RenderSystem::~RenderSystem() {
    if(_alive){
        Shutdown();
    }
    UnRegisterRenderSystem(this);
}

}
