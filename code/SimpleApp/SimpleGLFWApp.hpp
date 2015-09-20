#pragma once

#include "SimpleApp.hpp"

#include <GL\glew.h>

#include "ContextGLFW.hpp"

#include <windows.h>

#undef CreateWindow //FUCKING WINDOWS SDK, BURN IN HELL

namespace mr {

class SimpleGLFWApp : public SimpleApp {
public:
    static void glfwError(int level, const char* desc) {
        mr::Log::LogString("glfw error. level " + std::to_string(level) + ". " + std::string(desc), MR_LOG_LEVEL_ERROR);
    }

    bool ContextInit() override {
        glfwSetErrorCallback(glfwError);

        if(!glfwInit()){
            mr::Log::LogString("Failed glfwInit in SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        mr::ContextGLFWPtr mtCtx = nullptr;
        if(multithreaded) {
            hints.Setup(true);
            if((mtCtx = contextMgr.CreateWindow(1, 1, windowName.c_str())) == nullptr) {
                mr::Log::LogString("Failed glfw multithred window creation failed in SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
                contextMgr.Destroy();
                return false;
            }
        }

        context = nullptr;
        hints.Setup(false);
        if((context = contextMgr.CreateWindow(window_width, window_height, windowName.c_str(), mtCtx)) == nullptr) {
            mr::Log::LogString("Failed glfw main window creation failed in SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
            contextMgr.Destroy();
            return false;
        }

        context->MakeCurrent();

        return true;
    }

    bool Go(std::string const& WindowName = "MorglodsRender", unsigned short const& WindowWidth = 800, unsigned short const& WindowHeight = 600, GLFWWindowHints const& hints_ = GLFWWindowHints(), bool const& multithreaded = false) {
        hints = hints_;
        return SimpleApp::Go(WindowName, WindowWidth, WindowHeight, multithreaded);
    }

    void ContextDestroy() override {
        contextMgr.Destroy();
    }

    void MainLoop() override {
        float lTime = glfwGetTime();
        float delta = 0.00000001f;

        GLFWwindow* mainWindow = context->GetWindow();
        while(!glfwWindowShouldClose(mainWindow)) {
            Frame(delta);

            glfwPollEvents();
            glfwSwapBuffers(mainWindow);

            float nTime = glfwGetTime();
            delta = nTime - lTime;
            lTime = nTime;
        }
    }

    SimpleGLFWApp() = default;
    virtual ~SimpleGLFWApp() {}

    mr::ContextGLFWPtr context = nullptr;
    mr::ContextManagerGLFW contextMgr;
    GLFWWindowHints hints = GLFWWindowHints();
};

}
