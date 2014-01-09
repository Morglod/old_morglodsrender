#pragma once

#ifndef _MR_RENDER_WINDOW_H_
#define _MR_RENDER_WINDOW_H_

#include "pre.hpp"
#include "Events.hpp"

namespace MR {

class RenderWindow {
public:

    //sender - RenderWindow
    //arg1, arg2 - new x,y
    MR::Event<int&, int&> OnPosChanged;

    //sender - RenderWindow
    //arg1, arg2 - new w,h
    MR::Event<int&, int&> OnSizeChanged;

    //sender - RenderWindow
    //arg1 - new title
    MR::Event<const char*> OnTitleChanged;

    //sender - RenderWindow
    //arg1 - new refreshes num
    MR::Event<int&> OnSyncChanged;

    //sender - RenderWindow
    //arg1 - new state
    MR::Event<bool&> OnVisibilityChanged;

    //sender - RenderWindow
    //arg1 - unicode char of key
    MR::Event<unsigned int&> OnChar;

    //sender - RenderWindow
    //arg1 - glfw key GLFW_A, GLFW_B etc
    //arg2 - system scan code
    //arg3 - glfw key action GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
    //arg4 - glfw mods bit
    MR::Event<int&, int&, int&, int&> OnKey;

    //sender - RenderWindow
    //arg1 - glfw button
    //arg2 - glfw key action GLFW_PRESS, GLFW_RELEASE
    //arg3 - glfw mods bit
    MR::Event<int&, int&, int&> OnMouseButton;

    //sender - RenderWindow
    //arg1 - GL_TRUE or GL_FALSE
    MR::Event<int&> OnCursorEnterStateChanged;

    //sender - RenderWindow
    //arg1 - x
    //arg2 - y
    MR::Event<double&, double&> OnCursorPosChanged;

    //sender - RenderWindow
    //arg1 - x offset
    //arg2 - y offset
    MR::Event<double&, double&> OnScroll;

    //sender - RenderWindow
    //arg1, arg2 - new w,h
    MR::Event<int&, int&> OnFrameBufferSizeChanged;

    //sender - RenderWindow
    //arg1 - new monitor gamma
    MR::Event<float&> OnGammaChanged;

    //sender - RenderWindow
    //arg1 - nullptr
    MR::Event<void*> OnClose;

    //sender - RenderWindow
    //arg1 - GL_TRUE, GL_FALSE
    MR::Event<int&> OnFocusChanged;

    //sender - RenderWindow
    //arg1 - GL_TRUE, GL_FALSE
    MR::Event<int&> OnIconificationStateChanged;

    //sender - RenderWindow
    //arg1 - nullptr
    MR::Event<void*> OnRefresh;

protected:
    /* MR_DELTA_TYPE delta_last_time;
    MR_DELTA_TYPE delta; */

    GLFWwindow* glfw_handle;
    char* titlec;

    static void window_pos_callback(GLFWwindow* window, int x, int y);
    static void window_size_callback(GLFWwindow* window, int w, int h);
    static void window_char_callback(GLFWwindow* window, unsigned int c);
    static void window_cursor_entered_callback(GLFWwindow* window, int state);
    static void window_cursor_pos_changed_callback(GLFWwindow* window, double x, double y);
    static void window_key_callback(GLFWwindow* window, int glfw_key, int sys_scancode, int glfw_key_action, int glfw_mods);
    static void window_mouse_key_callback(GLFWwindow* window, int glfw_button, int glfw_key_action, int glfw_mods);
    static void window_scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
    static void window_close_callback(GLFWwindow* window);
    static void window_focus_changed_callback(GLFWwindow* window, int state);
    static void window_iconifed_callback(GLFWwindow* window, int state);
    static void window_refresh_callback(GLFWwindow* window);
    static void framebuffer_size_callback(GLFWwindow* window, int w, int h);

public:

    inline GLFWwindow* GetHandle() {
        return glfw_handle;
    }

    inline void GetPos(int* x, int* y){
        glfwGetWindowPos(glfw_handle, x, y);
    }

    inline void GetSize(int* w, int* h){
        glfwGetWindowSize(glfw_handle, w, h);
    }

    inline void GetFrameBufferSize(int* w, int* h){
        glfwGetFramebufferSize(glfw_handle, w, h);
    }

    inline const GLFWgammaramp* GetGLFWGammaRamp() {
        return glfwGetGammaRamp(glfwGetWindowMonitor(glfw_handle));
    }

    /*inline MR_DELTA_TYPE GetDelta(){
        return delta;
    }*/

    inline const char* GetTitle(){
        return titlec;
    }

    inline const char* GetClipboardString(){
        return glfwGetClipboardString(glfw_handle);
    }

    inline void SetPos(int x, int y){
        glfwSetWindowPos(glfw_handle, x, y);
        OnPosChanged(this, x, y);
    }

    inline void SetSize(int w, int h){
        glfwSetWindowSize(glfw_handle, w, h);
        OnSizeChanged(this, w, h);
    }

    inline void SetTitleC(const char* s){
        titlec = (char*)s;
        glfwSetWindowTitle(glfw_handle, s);
        OnTitleChanged(this, s);
    }

    inline void SetTitle(std::string s){
        SetTitleC(s.c_str());
    }

    inline void SetClipboardString(const char* s){
        glfwSetClipboardString(glfw_handle, s);
    }

    inline void SyncWait(int refreshes){
        glfwSwapInterval(refreshes);
        OnSyncChanged(this, refreshes);
    }

    inline void SetGamma(float gamma){
        glfwSetGamma(glfwGetWindowMonitor(glfw_handle), gamma);
        OnGammaChanged(this, gamma);
    }

    inline void Visible(bool state){
        if(state) glfwShowWindow(glfw_handle);
        else glfwHideWindow(glfw_handle);
        OnVisibilityChanged(this, state);
    }

    inline bool IsFocused(){
        return glfwGetWindowAttrib(glfw_handle, GLFW_FOCUSED);
    }

    inline bool IsIconified(){
        return glfwGetWindowAttrib(glfw_handle, GLFW_ICONIFIED);
    }

    inline bool IsVisible(){
        return glfwGetWindowAttrib(glfw_handle, GLFW_VISIBLE);
    }

    inline bool IsResizable(){
        return glfwGetWindowAttrib(glfw_handle, GLFW_RESIZABLE);
    }

    inline bool IsDecorated(){
        return glfwGetWindowAttrib(glfw_handle, GLFW_DECORATED);
    }

    inline bool ShouldClose(){
        return glfwWindowShouldClose(glfw_handle);
    }

    inline void Close(){
        glfwSetWindowShouldClose(glfw_handle, 1);
    }

    /*inline void BeginFrame(){
        delta = (MR_DELTA_TYPE)glfwGetTime() - fps_last_time;
        fps_last_time += delta;
    }*/

    inline void SwapBuffers(){
        glfwSwapBuffers(glfw_handle);
    }

    inline void MakeCurrent(){
        glfwMakeContextCurrent(glfw_handle);
    }

    struct RenderWindowCallbacks {
    public:
        bool cpos, //false
        csize, //true
        cchar, //true
        ccursor_enter, //false
        ccursor_pos, //false
        cframebuffer_size, //true
        ckey, //true
        cmouse_button, //true
        cscroll, //false
        cclose, //true
        cfocus, //false
        ciconify, //false
        crefresh; //false

        RenderWindowCallbacks();
    };

    struct RenderWindowHints {
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

        RenderWindowHints();
    };

    RenderWindow(std::string title, int width, int height, RenderWindowHints* hints = new RenderWindowHints(), GLFWwindow* parent_share_resources = NULL, RenderWindowCallbacks* callbacks = new RenderWindowCallbacks());
    ~RenderWindow();

    static RenderWindow* Create(std::string title, int width, int height) {
        return new RenderWindow(title, width, height);
    }
};

}

#endif
