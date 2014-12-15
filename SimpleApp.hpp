#pragma once

#ifndef _MR_SIMPLE_APP_H_
#define _MR_SIMPLE_APP_H_

#include "Context.hpp"
#include "Utils/Exception.hpp"
//#include "Pipeline.hpp"
#include "ConfigClass.hpp"
#include "Config.hpp"
#include "Core.hpp"
#include "Scene/PerspectiveCamera.hpp"
#include "Utils/Log.hpp"

#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

#include <iostream>
#include <fstream>
#include <windows.h>

std::ofstream __simple_app__log_file("log.txt");

namespace mr {
    class SimpleApp;
}

class GLFWContext : public mr::IContext {
    friend class mr::SimpleApp;
public:
    void Destroy() override {
        mr::Shutdown();

        MakeNullCurrent();
        glfwDestroyWindow(window);
        glfwDestroyWindow(mt_window);
        glfwTerminate();
    }

    void MakeCurrent() override {
        glfwMakeContextCurrent(window);
    }

    void MakeMultithreadCurrent() override {
        glfwMakeContextCurrent(mt_window);
    }

    void MakeNullCurrent() override {
        glfwMakeContextCurrent(0);
    }


    bool IsMultithread() override {
        return (mt_window != 0);
    }

    void SwapBuffers() override {
        glfwSwapBuffers(window);
    }

    bool ExtensionSupported(const char* ext) override {
        return (glfwExtensionSupported(ext) == GL_TRUE);
    }

    ProcFunc GetProcAddress(const char* procname) override {
        return (ProcFunc)glfwGetProcAddress(procname);
    }

    void GetWindowSizes(int& width, int& height) override {
        glfwGetWindowSize(window, &width, &height);
    }

    inline GLFWwindow* GetMainWindow() { return window; }

    GLFWContext() {
        mr::IContext::Current = dynamic_cast<mr::IContext*>(this);
    }

    virtual ~GLFWContext() {
        Destroy();
        mr::IContext::Current = nullptr;
    }
protected:
    GLFWwindow *window = 0, *mt_window = 0;
};

namespace mr {

class WindowHints {
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

    inline void Configure(mu::Config* cfg);
    inline void SaveConfig(mu::Config* cfg);

    inline WindowHints();
};

class SimpleApp {
public:
    static void LogString(const std::string& s, const int& level) {
        switch(level) {
        case MR_LOG_LEVEL_ERROR:
            std::cout << "Error: ";
            __simple_app__log_file << "Error: ";
            break;
        case MR_LOG_LEVEL_EXCEPTION:
            std::cout << "Exception: ";
            __simple_app__log_file << "Exception: ";
            break;
        case MR_LOG_LEVEL_INFO:
            std::cout << "Info: ";
            __simple_app__log_file << "Info: ";
            break;
        case MR_LOG_LEVEL_WARNING:
            std::cout << "Warning: ";
            __simple_app__log_file << "Warning: ";
            break;
        }
        std::cout << s << std::endl;
        __simple_app__log_file << s << std::endl;
        __simple_app__log_file.flush();
    }

    static void glfwError(int level, const char* desc) {
        mr::Log::LogString("glfw error. level " + std::to_string(level) + ". " + std::string(desc), MR_LOG_LEVEL_ERROR);
    }

    virtual bool Go( const std::string& WindowName = "MorglodsRender", const unsigned short& WindowWidth = 800, const unsigned short& WindowHeight = 600, const WindowHints& hints = WindowHints(), const bool& multithreaded = false) {
        aspect = (float)WindowWidth / (float)WindowHeight;
        window_width = WindowWidth;
        window_height = WindowHeight;

        mr::Log::Add(LogString);
        glfwSetErrorCallback(glfwError);

        if(!glfwInit()){
            mr::Log::LogString("Failed glfwInit in SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
        }

        glfwWindowHint(GLFW_OPENGL_PROFILE, hints.opengl_profile);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, hints.opengl_debug_context);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, hints.opengl_forward_compat);
        glfwWindowHint(GLFW_CLIENT_API, hints.client_api);
        glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, hints.context_robustness);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, hints.context_version_major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, hints.context_version_minor);

        if(multithreaded) {
            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
            glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
            context.mt_window = glfwCreateWindow(1, 1, (WindowName+" Multithreaded").c_str(), NULL, NULL);
            if(!context.mt_window) {
                mr::Log::LogString("glfw multithred window creation failed", MR_LOG_LEVEL_ERROR);
                glfwTerminate();
                return false;
            }
        }

        glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, hints.accum_alpha_bits);
        glfwWindowHint(GLFW_ACCUM_BLUE_BITS, hints.accum_blue_bits);
        glfwWindowHint(GLFW_ACCUM_GREEN_BITS, hints.accum_green_bits);
        glfwWindowHint(GLFW_ACCUM_RED_BITS, hints.accum_red_bits);
        glfwWindowHint(GLFW_ALPHA_BITS, hints.alpha_bits);
        glfwWindowHint(GLFW_AUX_BUFFERS, hints.aux_buffers);
        glfwWindowHint(GLFW_BLUE_BITS, hints.blue_bits);
        glfwWindowHint(GLFW_DECORATED, hints.decorated);
        glfwWindowHint(GLFW_DEPTH_BITS, hints.depth_bits);
        glfwWindowHint(GLFW_GREEN_BITS, hints.green_bits);
        glfwWindowHint(GLFW_RED_BITS, hints.red_bits);
        glfwWindowHint(GLFW_REFRESH_RATE, hints.refresh_rate);
        glfwWindowHint(GLFW_RESIZABLE, hints.resizable);
        glfwWindowHint(GLFW_SAMPLES, hints.samples);
        glfwWindowHint(GLFW_SRGB_CAPABLE, hints.srgb_capable);
        glfwWindowHint(GLFW_STENCIL_BITS, hints.stencil_bits);
        glfwWindowHint(GLFW_STEREO, hints.stereo);
        glfwWindowHint(GLFW_VISIBLE, hints.visible);

        context.window = glfwCreateWindow(window_width, window_height, WindowName.c_str(), 0, context.mt_window);
        if(!context.window) {
            mr::Log::LogString("Failed window creation in SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        context.MakeCurrent();

        if(!mr::Init(&context)) {
            if(ThrowExceptions()) {
                throw mr::Exception("Failed context initialization SimpleApp::Go. Check log.");
            }
            return false;
        }

        camera = new mr::PerspectiveCamera( mr::Transform::WorldForwardVector() * 2.0f, glm::vec3(0,0,0), 90.0f, aspect, 0.1f, 500.0f);
        camera->SetAutoRecalc(true);
        //camera->SetCameraMode(MR::Camera::CameraMode::Direction);
        //MR::RenderManager::GetInstance()->UseCamera(camera); //set this camera as default

        //scene.AddCamera(camera);
        //scene.SetMainCamera(camera);

        /*pipeline = new MR::ForwardRenderingPipeline();//new MR::DefferedRenderingPipeline()
        if(!pipeline->Setup(&context)) {
            if(ThrowExceptions()) {
                throw MR::Exception("Failed RenderingPipeline::Setup.");
            }
            return false;
        }*/

        //pipeline->SetScene(&scene);

        if(!Setup()) {
            if(ThrowExceptions()) {
                throw mr::Exception("Failed SimpleApp::Setup.");
            }
            return false;
        }

        float lTime = glfwGetTime();
        float delta = 0.00000001f;

        while(!glfwWindowShouldClose(context.window)) {
            Frame(delta);

            glfwPollEvents();
            context.SwapBuffers();

            float nTime = glfwGetTime();
            delta = nTime - lTime;
            lTime = nTime;
        }

        Free();
        //pipeline->Shutdown();

        delete camera;
        //delete pipeline;

        return true;
    }

    virtual bool Setup() {
        return true;
    }

    virtual void Frame(const float& delta) {
        //scene.Draw();
    }

    virtual void Free() {
    }

    SimpleApp() : window_width(1), window_height(1), aspect(1.0f) {}
    virtual ~SimpleApp() {}

    GLFWContext context;
protected:
    unsigned short window_width, window_height;
    float aspect;

    mr::ICamera* camera;
    //MR::SceneManager scene;

    //MR::IRenderingPipeline* pipeline;
};

}

mr::WindowHints::WindowHints() :
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

void mr::WindowHints::Configure(mu::Config* cfg) {
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

void mr::WindowHints::SaveConfig(mu::Config* cfg) {
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

#endif // _MR_SIMPLE_APP_H_
