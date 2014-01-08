/*
MorglodsRender library header.
-Render framework, uses OpenGL.

3rd:
OpenGL
GLFW http://www.glfw.org/
SOIL (Simple Image Library) http://www.lonesock.net/soil.html
Assimp
GLEW GCC REBUILD
*/

#pragma once

#ifndef _MR_MORGLODS_RENDER_H_
#define _MR_MORGLODS_RENDER_H_

#include "pre.hpp"

#include "ResourceManager.hpp"
#include "Texture.hpp"
#include "Log.hpp"
//#include "Font.h"
//#include "Node.h"

#include "GeometryBuffer.hpp"
#include "Shader.hpp"
//#include "Scene.h"
#include "Mesh.hpp"
#include "Material.hpp"
//#include "RenderTarget.h"
#include "Camera.hpp"
//#include "Culling.h"
//#include "Optimizations.h"
//#include "AssimpImporter.h"
//#include "Collada.h"
#include "RenderManager.hpp"

#include "Events.hpp"
#include "MachineInfo.hpp"

#include "Transformation.hpp"

namespace MR{
    class Render{
    protected:
        GLFWwindow* window;
        bool released;
    public:
        inline GLFWwindow* GetWindow(){
            return window;
        }

        //-----------------------------------------------------------------------------------------------------
        //Inits MorglodsRender, Returns glfw window or NULL if failed and write error code to MR::LastErrorCode
        //window_name - Caption of new window
        //width - Width of new window
        //height - Height of new window
        bool Init(std::string window_name, int width, int height);

        //-------------------------
        //Stops MorglodsRender work
        void Shutdown();

        Render();
        ~Render();

        //----------------------------
        //Calculates frames per second
        //timeType is data type of time in calculations
        template<typename timeType>
        static unsigned short inline FPS(){
            static timeType lastTime = 0.0;
            static timeType time = 0.0;
            static unsigned short frames = 0;
            static unsigned short lastFps = 0;

            timeType currentTime = (timeType)glfwGetTime();
            time += currentTime-lastTime;
            lastTime = currentTime;
            ++frames;
            if(time > 1){
                lastFps = frames;
                time -= 1.0;
                frames = 0;
            }

            return lastFps;
        }
    };
}

#endif // _MR_MORGLODS_RENDER_H_
