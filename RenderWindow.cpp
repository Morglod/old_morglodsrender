#include "RenderWindow.hpp"
#include "Log.hpp"
#include "MachineInfo.hpp"

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

RenderWindow::RenderWindow(const std::string& title, const int& width, const int& height, const RenderWindowHints& hints, const RenderWindowCallbacks& callbacks, GLFWwindow* parent_share_resources) :
    glfw_handle(nullptr), titlec((char*)title.c_str()) {


    glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, hints.accum_alpha_bits);
    glfwWindowHint(GLFW_ACCUM_BLUE_BITS, hints.accum_blue_bits);
    glfwWindowHint(GLFW_ACCUM_GREEN_BITS, hints.accum_green_bits);
    glfwWindowHint(GLFW_ACCUM_RED_BITS, hints.accum_red_bits);
    glfwWindowHint(GLFW_ALPHA_BITS, hints.alpha_bits);
    glfwWindowHint(GLFW_AUX_BUFFERS, hints.aux_buffers);
    glfwWindowHint(GLFW_BLUE_BITS, hints.blue_bits);
    glfwWindowHint(GLFW_CLIENT_API, hints.client_api);
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, hints.context_robustness);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, hints.context_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, hints.context_version_minor);
    glfwWindowHint(GLFW_DECORATED, hints.decorated);
    glfwWindowHint(GLFW_DEPTH_BITS, hints.depth_bits);
    glfwWindowHint(GLFW_GREEN_BITS, hints.green_bits);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, hints.opengl_debug_context);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, hints.opengl_forward_compat);
    glfwWindowHint(GLFW_RED_BITS, hints.red_bits);
    glfwWindowHint(GLFW_REFRESH_RATE, hints.refresh_rate);
    glfwWindowHint(GLFW_RESIZABLE, hints.resizable);
    glfwWindowHint(GLFW_SAMPLES, hints.samples);
    glfwWindowHint(GLFW_SRGB_CAPABLE, hints.srgb_capable);
    glfwWindowHint(GLFW_STENCIL_BITS, hints.stencil_bits);
    glfwWindowHint(GLFW_STEREO, hints.stereo);
    glfwWindowHint(GLFW_VISIBLE, hints.visible);


    glfw_handle = glfwCreateWindow(width, height, titlec, NULL, parent_share_resources);

    if(glfw_handle) {
        MakeCurrent();

        if(parent_share_resources == NULL) {
            GLenum result = glewInit();
            if(result != GLEW_OK) {
                MR::Log::LogString("glew initialization failed", MR_LOG_LEVEL_ERROR);
                throw std::exception();
            }
            if(MR::MachineInfo::gl_version() == MR::MachineInfo::GLVersion::VNotSupported) {
                MR::Log::LogString("current opengl version (\""+MR::MachineInfo::gl_version_string()+"\") is not supported", MR_LOG_LEVEL_WARNING);
            }
            glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }

        glfwSetWindowUserPointer(glfw_handle, this);

        if(callbacks.cpos) glfwSetWindowPosCallback(glfw_handle, window_pos_callback);
        if(callbacks.csize) glfwSetWindowSizeCallback(glfw_handle, window_size_callback);
        if(callbacks.cchar) glfwSetCharCallback(glfw_handle, window_char_callback);
        if(callbacks.ccursor_enter) glfwSetCursorEnterCallback(glfw_handle, window_cursor_entered_callback);
        if(callbacks.ccursor_pos) glfwSetCursorPosCallback(glfw_handle, window_cursor_pos_changed_callback);
        if(callbacks.cframebuffer_size) glfwSetFramebufferSizeCallback(glfw_handle, framebuffer_size_callback);
        if(callbacks.ckey) glfwSetKeyCallback(glfw_handle, window_key_callback);
        if(callbacks.cmouse_button) glfwSetMouseButtonCallback(glfw_handle, window_mouse_key_callback);
        if(callbacks.cscroll) glfwSetScrollCallback(glfw_handle, window_scroll_callback);
        if(callbacks.cclose) glfwSetWindowCloseCallback(glfw_handle, window_close_callback);
        if(callbacks.cfocus) glfwSetWindowFocusCallback(glfw_handle, window_focus_changed_callback);
        if(callbacks.ciconify) glfwSetWindowIconifyCallback(glfw_handle, window_iconifed_callback);
        if(callbacks.crefresh) glfwSetWindowRefreshCallback(glfw_handle, window_refresh_callback);
    } else {
        MR::Log::LogString("glfw window creation failed", MR_LOG_LEVEL_ERROR);
        glfwTerminate();
    }
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
