#pragma once

#ifndef _MR_RENDER_WINDOW_H_
#define _MR_RENDER_WINDOW_H_

#include "Utils/Events.hpp"
#include "Types.hpp"
#include "GL/Context.hpp"

class GLFWwindow;
class GLFWgammaramp;

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

class RenderWindow : public virtual IRenderWindow, public MR::Object {
public:

    /** EVENTS **/


    //sender - RenderWindow
    //arg1 - new refreshes num
    MR::EventListener<RenderWindow*, const int&> OnSyncChanged;

    //sender - RenderWindow
    //arg1 - nullptr
    MR::EventListener<RenderWindow*, void*> OnRefresh;

    bool Init(bool multithreaded = false) override;
    void Destroy() override;

    void Close(const bool& safe = true) override;

    inline GLFWwindow* GetHandle() { return glfw_handle; }

    bool GetMouseButtonDown(const int& button) override;

    void GetPos(int* x, int* y) override;
    void GetSize(int* w, int* h) override;
    void GetFrameBufferSize(int* w, int* h);
    const GLFWgammaramp* GetGLFWGammaRamp();
    inline const char* GetTitle() override { return titlec; }

    void SetPos(const int& x, const int& y);
    void SetSize(const int& w, const int& h);

    void SetTitleC(const char* s);
    inline void SetTitle(const std::string& s){ SetTitleC(s.c_str()); }

    void SetClipboardString(const char* s);
    const char* GetClipboardString() override;

    void SyncWait(const int& refreshes);
    void SetGamma(const float& gamma);
    void Visible(const bool& state);

    void SetMousePos(const double& x, const double& y);
    void GetMousePos(double* x, double* y) override;

    bool IsFocused() override;
    bool IsIconified() override;
    bool IsVisible() override;
    bool IsResizable();
    bool IsDecorated();
    bool ShouldClose();
    void SwapBuffers() override;
    void MakeCurrent() override;

    void* _GetMultithreadContext() override {
        return glfw_handle_multithread;
    }

    bool ExtensionSupported(const char* ext) override;
    ProcFunc GetProcAddress(const char* procname) override;

    void ResetViewport(IRenderSystem* rs) override;

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

    RenderWindow(const std::string& title, const int& width, const int& height);
    RenderWindow(const std::string& title, const int& width, const int& height, const RenderWindowHints& hints, const RenderWindowCallbacks& callbacks = RenderWindowCallbacks());
    ~RenderWindow();

    static RenderWindow* Create(const std::string& title, const int& width, const int& height) { return new RenderWindow(title, width, height); }

protected:
    GLFWwindow* glfw_handle;
    GLFWwindow* glfw_handle_multithread = 0; //invivislbe window for second context, if initializated as multithreaded
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

    RenderWindowHints _creationHints;
    RenderWindowCallbacks _creationCallbacks;
    int _creationWidth;
    int _creationHeight;

    bool _inited;
};

}

#endif
