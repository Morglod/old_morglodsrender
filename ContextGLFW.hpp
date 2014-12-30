#pragma once

#include "ContextManager.hpp"

#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

namespace mr {

class GLFWWindowHints {
public:
    int resizable, //GL_FALSE
        visible, //GL_TRUE
        decorated, //GL_TRUE
        red_bits, //8
        green_bits, //8
        blue_bits, //8
        alpha_bits, //8
        depth_bits, //24
        stencil_bits, //8
        accum_red_bits, //0
        accum_green_bits, //0
        accum_blue_bits, //0
        accum_alpha_bits, //0
        aux_buffers, //0
        samples, //0
        refresh_rate, //0
        stereo, //GL_FALSE
        srgb_capable, //GL_FALSE
        client_api, //GLFW_OPENGL_API
        context_version_major, //1
        context_version_minor, //0
        context_robustness, //GLFW_NO_ROBUSTNESS
        opengl_forward_compat, //GL_FALSE
        opengl_debug_context, //GL_FALSE
        opengl_profile; //GLFW_OPENGL_ANY_PROFILE

    inline void Setup(bool invisibleWindow = false) const;

    inline void Configure(mu::Config* cfg);
    inline void SaveConfig(mu::Config* cfg);

    inline GLFWWindowHints();
};

class ContextGLFW : public IContext {
public:

    void MakeCurrent() override { glfwMakeContextCurrent(_window); }
    void SwapBuffers() override { glfwSwapBuffers(_window); }
    void GetWindowSizes(int& width, int& height) override { glfwGetWindowSize(_window, &width, &height); }
    inline GLFWwindow* GetWindow() { return _window; }

    void Destroy() override {
        glfwDestroyWindow(_window);
    }

    ContextGLFW() : _window(nullptr) {}
    ContextGLFW(GLFWwindow* wnd) : _window(wnd) {}
    virtual ~ContextGLFW();
protected:
    GLFWwindow* _window;
};

typedef std::shared_ptr<ContextGLFW> ContextGLFWPtr;

class ContextManagerGLFW : public virtual mr::IContextManager {
public:
    inline ContextGLFWPtr CreateWindow(unsigned short windowWidth, unsigned short windowHeight, std::string const& windowName, ContextGLFWPtr parentWindow = nullptr) {
        auto wnd = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), 0, (parentWindow != nullptr) ? parentWindow.get()->GetWindow() : 0);
        if(!wnd) return nullptr;
        auto glfwContext = new ContextGLFW(wnd);
        _ctx.push_back(dynamic_cast<IContext*>(glfwContext));
        return ContextGLFWPtr(glfwContext);
    }

    IContext* GetDefaultContext() { return _ctx[_defaultCtxIndex]; }
    unsigned int GetContextsNum() { return _ctx.size(); }
    IContext* GetContext(unsigned int const& contextIndex) { return _ctx[contextIndex]; }
    void MakeNullContextCurrent() { glfwMakeContextCurrent(0); }

    inline void SetDefaultContext(unsigned int const& contextIndex) { _defaultCtxIndex = contextIndex; }

    void Destroy() {
        mr::Shutdown();
        MakeNullContextCurrent();
        for(unsigned int i = 0; i < GetContextsNum(); ++i) {
            if(! _ctx[i]) continue;
            _ctx[i]->Destroy();
            _ctx[i] = nullptr;
        }
        _ctx.clear();
        glfwTerminate();
    }

    bool IsMultithread() { return (GetContextsNum() > 1); }

    bool ExtensionSupported(const char* ext) { return glfwExtensionSupported(ext); }
    ProcFunc GetProcAddress(const char* procname) { return (ProcFunc)glfwGetProcAddress(procname); }

    virtual ~ContextManagerGLFW();
private:
    unsigned int _defaultCtxIndex = 0;
    std::vector<IContext*> _ctx;
};

bool Init(ContextGLFWPtr glfwContext) {
    return Init(dynamic_cast<mr::IContext*>(glfwContext.get()));
}

}

//mr::ContextManagerGLFW::ContextManagerGLFW() {}
mr::ContextManagerGLFW::~ContextManagerGLFW() {}
mr::ContextGLFW::~ContextGLFW() {}

void mr::GLFWWindowHints::Setup(bool invisibleWindow) const {
    glfwWindowHint(GLFW_OPENGL_PROFILE, opengl_profile);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, opengl_debug_context);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, opengl_forward_compat);
    glfwWindowHint(GLFW_CLIENT_API, client_api);
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, context_robustness);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context_version_minor);

    if(invisibleWindow) {
        glfwWindowHint(GLFW_RESIZABLE, 0);
        glfwWindowHint(GLFW_VISIBLE, 0);
    } else {
        glfwWindowHint(GLFW_RESIZABLE, resizable);
        glfwWindowHint(GLFW_VISIBLE, visible);
    }

    glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, accum_alpha_bits);
    glfwWindowHint(GLFW_ACCUM_BLUE_BITS, accum_blue_bits);
    glfwWindowHint(GLFW_ACCUM_GREEN_BITS, accum_green_bits);
    glfwWindowHint(GLFW_ACCUM_RED_BITS, accum_red_bits);
    glfwWindowHint(GLFW_ALPHA_BITS, alpha_bits);
    glfwWindowHint(GLFW_AUX_BUFFERS, aux_buffers);
    glfwWindowHint(GLFW_BLUE_BITS, blue_bits);
    glfwWindowHint(GLFW_DECORATED, decorated);
    glfwWindowHint(GLFW_DEPTH_BITS, depth_bits);
    glfwWindowHint(GLFW_GREEN_BITS, green_bits);
    glfwWindowHint(GLFW_RED_BITS, red_bits);
    glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate);
    glfwWindowHint(GLFW_SAMPLES, samples);
    glfwWindowHint(GLFW_SRGB_CAPABLE, srgb_capable);
    glfwWindowHint(GLFW_STENCIL_BITS, stencil_bits);
    glfwWindowHint(GLFW_STEREO, stereo);
}

mr::GLFWWindowHints::GLFWWindowHints() :
    resizable(GL_FALSE),
    visible(GL_TRUE),
    decorated(GL_TRUE),
    red_bits(8),
    green_bits(8),
    blue_bits(8),
    alpha_bits(8),
    depth_bits(24),
    stencil_bits(8),
    accum_red_bits(0),
    accum_green_bits(0),
    accum_blue_bits(0),
    accum_alpha_bits(0),
    aux_buffers(0),
    samples(0),
    refresh_rate(0),
    stereo(GL_FALSE),
    srgb_capable(GL_FALSE),
    client_api(GLFW_OPENGL_API),
    context_version_major(3),
    context_version_minor(2),
    context_robustness(GLFW_NO_ROBUSTNESS),
    opengl_forward_compat(GL_TRUE),
    opengl_debug_context(GL_FALSE),
    opengl_profile(GLFW_OPENGL_CORE_PROFILE) {
}

void mr::GLFWWindowHints::Configure(mu::Config* cfg) {
	if(!cfg) return;

    cfg->GetTo("display.srgb_capable", srgb_capable);
    cfg->GetTo("display.stereo", stereo);
    cfg->GetTo("display.samples", samples);
    cfg->GetTo("window.visible", visible);
    cfg->GetTo("window.refresh_rate", refresh_rate);
    cfg->GetTo("window.resizable", resizable);
    cfg->GetTo("window.decorated", decorated);
    cfg->GetTo("context.debug", opengl_debug_context);
    cfg->GetTo("context.forward_compat", opengl_forward_compat);
    cfg->GetTo("context.profile", opengl_profile);
    cfg->GetTo("context.robustness", context_robustness);
    cfg->GetTo("context.version_major", context_version_major);
    cfg->GetTo("context.version_minor", context_version_minor);
    cfg->GetTo("context.client_api", client_api);
    cfg->GetTo("buffers.red_bits", red_bits);
    cfg->GetTo("buffers.green_bits", green_bits);
    cfg->GetTo("buffers.blue_bits", blue_bits);
    cfg->GetTo("buffers.alpha_bits", alpha_bits);
    cfg->GetTo("buffers.depth_bits", depth_bits);
    cfg->GetTo("buffers.stencil_bits", stencil_bits);
    cfg->GetTo("buffers.aux_buffers", aux_buffers);
    cfg->GetTo("buffers.accum_red_bits", accum_red_bits);
    cfg->GetTo("buffers.accum_green_bits", accum_green_bits);
    cfg->GetTo("buffers.accum_blue_bits", accum_blue_bits);
    cfg->GetTo("buffers.accum_alpha_bits", accum_alpha_bits);
}

void mr::GLFWWindowHints::SaveConfig(mu::Config* cfg) {
	if(!cfg) return;

    cfg->Set("display.srgb_capable", std::to_string(srgb_capable));
    cfg->Set("display.stereo", std::to_string(stereo));
    cfg->Set("display.samples", std::to_string(samples));
    cfg->Set("window.visible", std::to_string(visible));
    cfg->Set("window.refresh_rate", std::to_string(refresh_rate));
    cfg->Set("window.resizable", std::to_string(resizable));
    cfg->Set("window.decorated", std::to_string(decorated));
    cfg->Set("context.debug", std::to_string(opengl_debug_context));
    cfg->Set("context.forward_compat", std::to_string(opengl_forward_compat));
    cfg->Set("context.profile", std::to_string(opengl_profile));
    cfg->Set("context.robustness", std::to_string(context_robustness));
    cfg->Set("context.version_major", std::to_string(context_version_major));
    cfg->Set("context.version_minor", std::to_string(context_version_minor));
    cfg->Set("context.client_api", std::to_string(client_api));
    cfg->Set("buffers.red_bits", std::to_string(red_bits));
    cfg->Set("buffers.green_bits", std::to_string(green_bits));
    cfg->Set("buffers.blue_bits", std::to_string(blue_bits));
    cfg->Set("buffers.alpha_bits", std::to_string(alpha_bits));
    cfg->Set("buffers.depth_bits", std::to_string(depth_bits));
    cfg->Set("buffers.stencil_bits", std::to_string(stencil_bits));
    cfg->Set("buffers.aux_buffers", std::to_string(aux_buffers));
    cfg->Set("buffers.accum_red_bits", std::to_string(accum_red_bits));
    cfg->Set("buffers.accum_green_bits", std::to_string(accum_green_bits));
    cfg->Set("buffers.accum_blue_bits", std::to_string(accum_blue_bits));
    cfg->Set("buffers.accum_alpha_bits", std::to_string(accum_alpha_bits));
}
