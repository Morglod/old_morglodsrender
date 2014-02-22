#pragma once

#ifndef _MR_SIMPLE_APP_H_
#define _MR_SIMPLE_APP_H_

#include "RenderContext.hpp"
#include "RenderWindow.hpp"
#include "Exception.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

#include <iostream>
#include <windows.h>

namespace MR{

class SimpleApp {
public:
    static void LogString(const std::string& s, const int& level) {
        switch(level) {
        case MR_LOG_LEVEL_ERROR:
            std::cout << "Error: ";
            break;
        case MR_LOG_LEVEL_EXCEPTION:
            std::cout << "Exception: ";
            break;
        case MR_LOG_LEVEL_INFO:
            std::cout << "Info: ";
            break;
        case MR_LOG_LEVEL_WARNING:
            std::cout << "Warning: ";
            break;
        }
        std::cout << s << std::endl;
    }

    virtual bool Go( const std::string& WindowName = "MorglodsRender", const unsigned short& WindowWidth = 800, const unsigned short& WindowHeight = 600, const RenderWindow::RenderWindowHints& hints = RenderWindow::RenderWindowHints(), const RenderWindow::RenderWindowCallbacks& callbacks = RenderWindow::RenderWindowCallbacks() ){
        aspect = (float)WindowWidth / (float)WindowHeight;

        MR::Log::Add(LogString);

        if(!context.Init()){
            if(ThrowExceptions()){
                throw MR::Exception("Failed context initialization SimpleApp::Go. Check log");
            }
            return false;
        }
        window = new MR::RenderWindow(WindowName, WindowWidth, WindowHeight, hints, callbacks);

        camera = new MR::Camera( MR::Transform::WorldBackwardVector(), glm::vec3(0,0,0), 90.0f, 0.1f, 1000.0f, aspect);
        camera->SetAutoRecalc(true); //Matrixes will recalc automaticly, when pos/rot/etc of camera changed
        camera->SetCameraMode(MR::Camera::CameraMode::Direction);
        context.camera = camera; //set this camera as default

        scene.AddCamera(camera);
        scene.SetMainCamera(camera);

        if(!Setup()) {
            if(ThrowExceptions()){
                throw MR::Exception("Failed SimpleApp::Setup.");
            }
            return false;
        }

        float lTime = glfwGetTime();
        float delta = 0.00000001f;

        while(!window->ShouldClose()){
            Frame(delta);

            glfwPollEvents();
            window->SwapBuffers();

            float nTime = glfwGetTime();
            delta = nTime - lTime;
            lTime = nTime;
        }

        Free();
        delete camera;
        delete window;

        return true;
    }

    virtual bool Setup(){
        return true;
    }

    virtual void Frame(const float& delta){
        scene.Draw(context);
    }

    virtual void Free(){
    }

    virtual void CameraUniforms(MR::Shader* sh, const std::string& projMatrix, const std::string& viewMatrix, const std::string& mvp){
        sh->CreateUniform(projMatrix, camera->GetProjectMatrix());
        sh->CreateUniform(viewMatrix, camera->GetViewMatrix());
        sh->CreateUniform(mvp, camera->GetMVP());
    }

    SimpleApp() : window_width(1), window_height(1), aspect(1.0f), context(), window(NULL) {}
    virtual ~SimpleApp() {}
protected:
    unsigned short window_width, window_height;
    float aspect;

    MR::RenderContext context;
    MR::RenderWindow* window;

    MR::Camera* camera;
    MR::SceneManager scene;
};

}

#endif // _MR_SIMPLE_APP_H_
