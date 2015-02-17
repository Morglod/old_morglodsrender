#pragma once

#ifndef _MR_SIMPLE_APP_H_
#define _MR_SIMPLE_APP_H_

#include "Utils/Exception.hpp"
#include "ConfigClass.hpp"
#include "Config.hpp"
#include "Core.hpp"
#include "Scene/PerspectiveCamera.hpp"
#include "Utils/Log.hpp"

#define GLEW_STATIC
#include <GL\glew.h>

#include "ContextGLFW.hpp"

#include <iostream>
#include <fstream>
#include <windows.h>

#undef CreateWindow //FUCKING WINDOWS SDK, BURN IN HELL

std::ofstream __simple_app__log_file("log.txt");

namespace mr {

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

    virtual bool Go(std::string const& WindowName = "MorglodsRender", unsigned short const& WindowWidth = 800, unsigned short const& WindowHeight = 600, GLFWWindowHints const& hints = GLFWWindowHints(), bool const& multithreaded = false) {
        aspect = (float)WindowWidth / (float)WindowHeight;
        window_width = WindowWidth;
        window_height = WindowHeight;

        mr::Log::Add(LogString);
        glfwSetErrorCallback(glfwError);

        if(!glfwInit()){
            mr::Log::LogString("Failed glfwInit in SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
        }

        mr::ContextGLFWPtr mtCtx = nullptr;
        if(multithreaded) {
            hints.Setup(true);
            if((mtCtx = contextMgr.CreateWindow(1, 1, WindowName.c_str())) == nullptr) {
                mr::Log::LogString("Failed glfw multithred window creation failed in SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
                contextMgr.Destroy();
                return false;
            }
        }

        context = nullptr;
        hints.Setup(false);
        if((context = contextMgr.CreateWindow(window_width, window_height, WindowName.c_str(), mtCtx)) == nullptr) {
            mr::Log::LogString("Failed glfw main window creation failed in SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
            contextMgr.Destroy();
            return false;
        }

        context->MakeCurrent();

        if(!mr::Init()) {
            if(ThrowExceptions()) {
                throw mr::Exception("Failed context initialization SimpleApp::Go. Check log.");
            }
            mr::Log::LogString("Failed context initialization SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        camera = new mr::PerspectiveCamera( mr::Transform::WorldForwardVector() * 2.0f, glm::vec3(0,0,0), 90.0f, aspect, 0.1f, 500.0f);
        camera->SetAutoRecalc(true);

        if(!Setup()) {
            if(ThrowExceptions()) {
                throw mr::Exception("Failed SimpleApp::Setup.");
            }
            return false;
        }

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

        Free();

        delete camera;
        contextMgr.Destroy();

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

    mr::ContextGLFWPtr context = nullptr;
    mr::ContextManagerGLFW contextMgr;
protected:
    unsigned short window_width, window_height;
    float aspect;

    mr::ICamera* camera;
};

}

#endif // _MR_SIMPLE_APP_H_
