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

//GLEW
#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

//GLFW
#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

//ASSIMP
/*#ifndef AI_ASSIMP_H_INC
#   include <assimp/cimport.h>
#endif
#ifndef AI_MATERIAL_H_INC
#   include <assimp/material.h>
#endif
#ifndef __AI_SCENE_H_INC__
#   include <assimp/scene.h>
#endif
#ifndef AI_POSTPROCESS_H_INC
#   include <assimp/postprocess.h>
#endif
#ifndef INCLUDED_AI_ASSIMP_HPP
#   include <assimp/Importer.hpp>
#endif
#ifndef INCLUDED_AI_MESH_H
#   include <assimp/mesh.h>
#endif*/

#include <memory>
#include <stdlib.h>
#include <windows.h>
#include <iostream>

#include "ResourceManager.h"
#include "Texture.h"
#include "Log.h"
#include "Font.h"
//#include "Node.h"

#include "GeometryBuffer.h"
#include "Shader.h"
//#include "Scene.h"
//#include "Mesh.h"
#include "Material.h"
//#include "RenderTarget.h"
#include "Camera.h"
#include "Culling.h"
//#include "Optimizations.h"
//#include "AssimpImporter.h"
//#include "Collada.h"

#include "MachineInfo.h"

namespace MR{
    //-----------------------------------------------------------------------------------------------------
    //Inits MorglodsRender, Returns glfw window or NULL if failed and write error code to MR::LastErrorCode
    //window_name - Caption of new window
    //width - Width of new window
    //height - Height of new window
    GLFWwindow* Init(const char* window_name, int width, int height);

    //-------------------------
    //Stops MorglodsRender work
    void Shutdown();

    //----------------------------
    //Calculates frames per second
    //timeType is data type of time in calculations
    template<typename timeType>
    unsigned short inline FPS(){
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
}

#endif // _MR_MORGLODS_RENDER_H_
