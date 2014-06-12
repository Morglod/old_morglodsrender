#include "RenderWindow.hpp"
#include "Log.hpp"
#include "MachineInfo.hpp"
#include "RenderSystem.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

void ErrorHandle(int errorCode, const char* text){
    MR::Log::LogString("GLFW Error ["+std::to_string(errorCode)+"] : "+std::string(text), MR_LOG_LEVEL_ERROR);
}

namespace MR {

void RenderWindow::window_pos_callback(GLFWwindow* window, int x, int y) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnSizeChanged(p, x, y);
}

void RenderWindow::window_size_callback(GLFWwindow* window, int w, int h) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnSizeChanged(p, w, h);
}

void RenderWindow::window_char_callback(GLFWwindow* window, unsigned int c) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnChar(p, c);
}

void RenderWindow::window_cursor_entered_callback(GLFWwindow* window, int state) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnCursorEnterStateChanged(p, state);
}

void RenderWindow::window_cursor_pos_changed_callback(GLFWwindow* window, double x, double y) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnCursorPosChanged(p, x, y);
}

void RenderWindow::window_key_callback(GLFWwindow* window, int glfw_key, int sys_scancode, int glfw_key_action, int glfw_mods) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnKey(p, glfw_key, sys_scancode, glfw_key_action, glfw_mods);
}

void RenderWindow::window_mouse_key_callback(GLFWwindow* window, int glfw_button, int glfw_key_action, int glfw_mods) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnMouseButton(p, glfw_button, glfw_key_action, glfw_mods);
}

void RenderWindow::window_scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnScroll(p, x_offset, y_offset);
}

void RenderWindow::window_close_callback(GLFWwindow* window) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnClose(p, nullptr);
    p->Destroy();
}

void RenderWindow::window_focus_changed_callback(GLFWwindow* window, int state) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnFocusChanged(p, state);
}

void RenderWindow::window_iconifed_callback(GLFWwindow* window, int state) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnIconificationStateChanged(p, state);
}

void RenderWindow::window_refresh_callback(GLFWwindow* window) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnRefresh(p, nullptr);
}

void RenderWindow::framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    RenderWindow* p = (RenderWindow*)glfwGetWindowUserPointer(window);
    p->OnFrameBufferSizeChanged(p, w, h);
}

bool RenderWindow::Init(bool multithreaded) {
    if(_inited) return true;

    if(multithreaded) {
        glfw_handle_multithread = NULL;

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
        glfw_handle_multithread = glfwCreateWindow(1, 1, (std::string(titlec)+" Multithreaded").c_str(), NULL, NULL);
        if(!glfw_handle_multithread) {
            MR::Log::LogString("glfw multithred window creation failed", MR_LOG_LEVEL_ERROR);
            glfwTerminate();
            return false;
        }
    }

    glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, _creationHints.accum_alpha_bits);
    glfwWindowHint(GLFW_ACCUM_BLUE_BITS, _creationHints.accum_blue_bits);
    glfwWindowHint(GLFW_ACCUM_GREEN_BITS, _creationHints.accum_green_bits);
    glfwWindowHint(GLFW_ACCUM_RED_BITS, _creationHints.accum_red_bits);
    glfwWindowHint(GLFW_ALPHA_BITS, _creationHints.alpha_bits);
    glfwWindowHint(GLFW_AUX_BUFFERS, _creationHints.aux_buffers);
    glfwWindowHint(GLFW_BLUE_BITS, _creationHints.blue_bits);
    glfwWindowHint(GLFW_CLIENT_API, _creationHints.client_api);
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, _creationHints.context_robustness);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _creationHints.context_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _creationHints.context_version_minor);
    glfwWindowHint(GLFW_DECORATED, _creationHints.decorated);
    glfwWindowHint(GLFW_DEPTH_BITS, _creationHints.depth_bits);
    glfwWindowHint(GLFW_GREEN_BITS, _creationHints.green_bits);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, _creationHints.opengl_debug_context);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, _creationHints.opengl_forward_compat);
    glfwWindowHint(GLFW_RED_BITS, _creationHints.red_bits);
    glfwWindowHint(GLFW_REFRESH_RATE, _creationHints.refresh_rate);
    glfwWindowHint(GLFW_RESIZABLE, _creationHints.resizable);
    glfwWindowHint(GLFW_SAMPLES, _creationHints.samples);
    glfwWindowHint(GLFW_SRGB_CAPABLE, _creationHints.srgb_capable);
    glfwWindowHint(GLFW_STENCIL_BITS, _creationHints.stencil_bits);
    glfwWindowHint(GLFW_STEREO, _creationHints.stereo);
    glfwWindowHint(GLFW_VISIBLE, _creationHints.visible);

    glfw_handle = glfwCreateWindow(_creationWidth, _creationHeight, titlec, NULL, glfw_handle_multithread);

    if(glfw_handle) {
        MakeCurrent();

        glfwSetWindowUserPointer(glfw_handle, dynamic_cast<RenderWindow*>(this));

        if(_creationCallbacks.cpos) glfwSetWindowPosCallback(glfw_handle, window_pos_callback);
        if(_creationCallbacks.csize) glfwSetWindowSizeCallback(glfw_handle, window_size_callback);
        if(_creationCallbacks.cchar) glfwSetCharCallback(glfw_handle, window_char_callback);
        if(_creationCallbacks.ccursor_enter) glfwSetCursorEnterCallback(glfw_handle, window_cursor_entered_callback);
        if(_creationCallbacks.ccursor_pos) glfwSetCursorPosCallback(glfw_handle, window_cursor_pos_changed_callback);
        if(_creationCallbacks.cframebuffer_size) glfwSetFramebufferSizeCallback(glfw_handle, framebuffer_size_callback);
        if(_creationCallbacks.ckey) glfwSetKeyCallback(glfw_handle, window_key_callback);
        if(_creationCallbacks.cmouse_button) glfwSetMouseButtonCallback(glfw_handle, window_mouse_key_callback);
        if(_creationCallbacks.cscroll) glfwSetScrollCallback(glfw_handle, window_scroll_callback);
        if(_creationCallbacks.cclose) glfwSetWindowCloseCallback(glfw_handle, window_close_callback);
        if(_creationCallbacks.cfocus) glfwSetWindowFocusCallback(glfw_handle, window_focus_changed_callback);
        if(_creationCallbacks.ciconify) glfwSetWindowIconifyCallback(glfw_handle, window_iconifed_callback);
        if(_creationCallbacks.crefresh) glfwSetWindowRefreshCallback(glfw_handle, window_refresh_callback);

        _inited = true;

        return true;
    } else {
        MR::Log::LogString("glfw window creation failed", MR_LOG_LEVEL_ERROR);
        glfwTerminate();

        return false;
    }
}

void RenderWindow::Destroy(){
    if(glfw_handle) glfwDestroyWindow(glfw_handle);
}

void RenderWindow::Close(const bool& safe) {
    if(safe) glfwSetWindowShouldClose(glfw_handle, 1);
    else Destroy();
}

bool RenderWindow::GetMouseButtonDown(const int& button) {
    return glfwGetMouseButton(glfw_handle, button);
}

void RenderWindow::GetPos(int* x, int* y) {
    glfwGetWindowPos(glfw_handle, x, y);
}

void RenderWindow::GetSize(int* w, int* h) {
    glfwGetWindowSize(glfw_handle, w, h);
}

void RenderWindow::GetFrameBufferSize(int* w, int* h) {
    glfwGetFramebufferSize(glfw_handle, w, h);
}

const GLFWgammaramp* RenderWindow::GetGLFWGammaRamp() {
    return glfwGetGammaRamp(glfwGetWindowMonitor(glfw_handle));
}

const char* RenderWindow::GetClipboardString() {
    return glfwGetClipboardString(glfw_handle);
}

void RenderWindow::SetPos(const int& x, const int& y) {
    glfwSetWindowPos(glfw_handle, x, y);
    OnPosChanged(this, x, y);
}

void RenderWindow::SetSize(const int& w, const int& h) {
    glfwSetWindowSize(glfw_handle, w, h);
    OnSizeChanged(this, w, h);
}

void RenderWindow::SetTitleC(const char* s) {
    titlec = (char*)s;
    glfwSetWindowTitle(glfw_handle, s);
    OnTitleChanged(this, s);
}

void RenderWindow::SetClipboardString(const char* s) {
    glfwSetClipboardString(glfw_handle, s);
}

void RenderWindow::SyncWait(const int& refreshes) {
    glfwSwapInterval(refreshes);
    OnSyncChanged(this, refreshes);
}

void RenderWindow::SetGamma(const float& gamma) {
    glfwSetGamma(glfwGetWindowMonitor(glfw_handle), gamma);
    OnGammaChanged(this, gamma);
}

void RenderWindow::Visible(const bool& state) {
    if(state) glfwShowWindow(glfw_handle);
    else glfwHideWindow(glfw_handle);
    OnVisibilityChanged(this, state);
}

void RenderWindow::SetMousePos(const double& x, const double& y){
    glfwSetCursorPos(glfw_handle, x, y);
}

void RenderWindow::GetMousePos(double* x, double* y){
    glfwGetCursorPos(glfw_handle, x, y);
}

bool RenderWindow::IsFocused() {
    return glfwGetWindowAttrib(glfw_handle, GLFW_FOCUSED);
}
bool RenderWindow::IsIconified() {
    return glfwGetWindowAttrib(glfw_handle, GLFW_ICONIFIED);
}
bool RenderWindow::IsVisible() {
    return glfwGetWindowAttrib(glfw_handle, GLFW_VISIBLE);
}
bool RenderWindow::IsResizable() {
    return glfwGetWindowAttrib(glfw_handle, GLFW_RESIZABLE);
}
bool RenderWindow::IsDecorated() {
    return glfwGetWindowAttrib(glfw_handle, GLFW_DECORATED);
}
bool RenderWindow::ShouldClose() {
    return glfwWindowShouldClose(glfw_handle);
}
void RenderWindow::SwapBuffers() {
    glfwSwapBuffers(glfw_handle);
}
void RenderWindow::MakeCurrent() {
    glfwMakeContextCurrent(glfw_handle);
    GL::SetCurrent(this);
}

bool RenderWindow::ExtensionSupported(const char* ext) {
    return glfwExtensionSupported(ext);
}

GL::IContext::ProcFunc RenderWindow::GetProcAddress(const char* procname) {
    return glfwGetProcAddress(procname);
}

void RenderWindow::ResetViewport(IRenderSystem* rs){
    int w = 1, h = 1;
    GetSize(&w, &h);
    rs->SetViewport(0, 0, w, h);
}

RenderWindow::RenderWindow(const std::string& title, const int& width, const int& height) :
    glfw_handle(nullptr), titlec((char*)title.c_str()), _creationWidth(width), _creationHeight(height), _inited(false) {
    glfwSetErrorCallback(ErrorHandle);
}

RenderWindow::RenderWindow(const std::string& title, const int& width, const int& height, const RenderWindowHints& hints, const RenderWindowCallbacks& callbacks) :
    glfw_handle(nullptr), titlec((char*)title.c_str()), _creationHints(hints), _creationCallbacks(callbacks), _creationWidth(width), _creationHeight(height), _inited(false) {
    glfwSetErrorCallback(ErrorHandle);
}

RenderWindow::~RenderWindow() {
    free((void*)titlec);
}

RenderWindow::RenderWindowCallbacks::RenderWindowCallbacks() :
    cpos(true),
    csize(true),
    cchar(true),
    ccursor_enter(false),
    ccursor_pos(false),
    cframebuffer_size(true),
    ckey(true),
    cmouse_button(true),
    cscroll(false),
    cclose(true),
    cfocus(false),
    ciconify(false),
    crefresh(false) {
}

RenderWindow::RenderWindowHints::RenderWindowHints() :
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
    context_version_major(1),
    context_version_minor(0),
    context_robustness(GLFW_NO_ROBUSTNESS),
    opengl_forward_compat(GL_FALSE),
    opengl_debug_context(GL_FALSE),
    opengl_profile(GLFW_OPENGL_ANY_PROFILE) {
}

}
