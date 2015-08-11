#pragma once

#include "Utils/Exception.hpp"
#include <mu/ConfigClass.hpp>
#include "Config.hpp"
#include "Core.hpp"
#include "Scene/PerspectiveCamera.hpp"
#include "Utils/Log.hpp"

#include <iostream>
#include <fstream>

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

    virtual bool ContextInit() {
        return false;
    }

    virtual void ContextDestroy() {
    }

    virtual void MainLoop() {
    }

    bool Go(std::string const& WindowName = "MorglodsRender", unsigned short const& WindowWidth = 800, unsigned short const& WindowHeight = 600, bool const& multithreaded_ = false) {
        aspect = (float)WindowWidth / (float)WindowHeight;
        window_width = WindowWidth;
        window_height = WindowHeight;
        multithreaded = multithreaded_;
        windowName = WindowName;

        mr::Log::Add(LogString);

        if(!ContextInit()) {
            mr::Log::LogString("Failed ContextInit in SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        if(!mr::Init()) {
            if(ThrowExceptions()) {
                throw mr::Exception("Failed render initialization SimpleApp::Go. Check log.");
            }
            mr::Log::LogString("Failed render initialization SimpleApp::Go.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        camera = new mr::PerspectiveCamera( mr::Transform::WorldForwardVector() * 2.0f, glm::vec3(0,0,0), 90.0f, aspect, 0.1f, 500.0f);
        camera->SetAutoRecalc(true);

        if(!Setup()) {
            if(ThrowExceptions()) {
                throw mr::Exception("Failed SimpleApp::Setup.");
            }
            mr::Log::LogString("Failed SimpleApp::Setup.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        MainLoop();
        Free();

        delete camera;
        ContextDestroy();

        return true;
    }

    virtual bool Setup() {
        return true;
    }

    virtual void Frame(const float& delta) {
    }

    virtual void Free() {
    }

    SimpleApp() : window_width(1), window_height(1), aspect(1.0f) {}
    virtual ~SimpleApp() {}
protected:
    unsigned short window_width, window_height;
    float aspect;
    bool multithreaded = false;
    std::string windowName = "";

    mr::ICamera* camera;
};

}

