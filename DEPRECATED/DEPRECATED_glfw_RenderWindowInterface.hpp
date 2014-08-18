#pragma once

#ifndef _MR_RENDER_WINDOW_INTERFACE_H_
#define _MR_RENDER_WINDOW_INTERFACE_H_

#include "Utils/Events.hpp"
#include "GL/Context.hpp"

namespace MR {

class IRenderSystem;

class IRenderWindow : public virtual GL::IContext {
public:
    /** EVENTS **/

    //sender - RenderWindow
    //arg1, arg2 - new x,y
    MR::EventListener<IRenderWindow*, const int&, const int&> OnPosChanged;

    //sender - RenderWindow
    //arg1, arg2 - new w,h
    MR::EventListener<IRenderWindow*, const int&, const int&> OnSizeChanged;

    //sender - RenderWindow
    //arg1 - new title
    MR::EventListener<IRenderWindow*, const char*> OnTitleChanged;

    //sender - RenderWindow
    //arg1 - new state
    MR::EventListener<IRenderWindow*, const bool&> OnVisibilityChanged;

    //sender - RenderWindow
    //arg1 - unicode char of key
    MR::EventListener<IRenderWindow*, const unsigned int&> OnChar;

    //sender - RenderWindow
    //arg1 - glfw key GLFW_A, GLFW_B etc
    //arg2 - system scan code
    //arg3 - glfw key action GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
    //arg4 - glfw mods bit
    MR::EventListener<IRenderWindow*, const int&, const int&, const int&, const int&> OnKey;

    //sender - RenderWindow
    //arg1 - glfw button
    //arg2 - glfw key action GLFW_PRESS, GLFW_RELEASE
    //arg3 - glfw mods bit
    MR::EventListener<IRenderWindow*, const int&, const int&, const int&> OnMouseButton;

    //sender - RenderWindow
    //arg1 - GL_TRUE or GL_FALSE
    MR::EventListener<IRenderWindow*, const int&> OnCursorEnterStateChanged;

    //sender - RenderWindow
    //arg1 - x
    //arg2 - y
    MR::EventListener<IRenderWindow*, const double&, const double&> OnCursorPosChanged;

    //sender - RenderWindow
    //arg1 - x offset
    //arg2 - y offset
    MR::EventListener<IRenderWindow*, const double&, const double&> OnScroll;

    //sender - RenderWindow
    //arg1, arg2 - new w,h
    MR::EventListener<IRenderWindow*, const int&, const int&> OnFrameBufferSizeChanged;

    //sender - RenderWindow
    //arg1 - new monitor gamma
    MR::EventListener<IRenderWindow*, const float&> OnGammaChanged;

    //sender - RenderWindow
    //arg1 - nullptr
    MR::EventListener<IRenderWindow*, void*> OnClose;

    //sender - RenderWindow
    //arg1 - GL_TRUE, GL_FALSE
    MR::EventListener<IRenderWindow*, const int&> OnFocusChanged;

    //sender - RenderWindow
    //arg1 - GL_TRUE, GL_FALSE
    MR::EventListener<IRenderWindow*, const int&> OnIconificationStateChanged;

    /** WINDOW **/

    virtual void Close(const bool& safe) = 0;

    virtual void GetPos(int* x, int* y) = 0;
    virtual void GetSize(int* w, int* h) = 0;

    virtual const char* GetTitle() = 0;

    virtual void ResetViewport(IRenderSystem* rs) = 0;

    /** INPUT **/

    virtual void GetMousePos(double* x, double* y) = 0;

    //button - mouse button num
    //0 - left
    //1 - middle
    //2 - right
    virtual bool GetMouseButtonDown(const int& button) = 0;

    virtual const char* GetClipboardString() = 0;

    /** STATE **/

    virtual bool IsFocused() = 0;
    virtual bool IsIconified() = 0;
    virtual bool IsVisible() = 0;
};

}

#endif
