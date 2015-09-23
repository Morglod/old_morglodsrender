#include "../mr/mr.hpp"

#include <GLFW/glfw3.h>
#include <thread>
#include <iostream>

struct Vertex {
    char word[3];
};

void main_logic(GLFWwindow* window) {
    using namespace mr;

    Core::Exec([](void*){ Info::PrintInfo(); }).wait();

    static Vertex vertexData[] = {
        { 'a', 'b', '\0' },
        { '1', '2', '\0' }
    };
    const auto vertexDataMem = Memory::Ref(vertexData, sizeof(vertexData));

    Buffer::CreationCmd cmd;
    cmd.map_after_creation = true;
    auto buf = Buffer::Create(vertexDataMem, cmd);

    auto vdecl = VertexDecl::Create();
    auto vdef = vdecl->Begin();
    vdef.Pos()
        .Color()
        .End();

    auto vbuffer = VertexBuffer::Create(buf.get(), vdecl);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

class glfwContextMgr : public mr::ContextMgr {
public:
    void* mainWindow;

    void MakeCurrent(void* context) override {
        glfwMakeContextCurrent((GLFWwindow*)context);
    }

    void* GetMainContext() override {
        return mainWindow;
    }

    glfwContextMgr(void* wnd) : mainWindow(wnd) {}
};

int main() {
    if(!glfwInit()) {
        MR_LOG_ERROR(glfwInit, "glfw init failed");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    GLFWwindow* window = glfwCreateWindow(800, 600,
                                          "MR2 Simple Cube",
                                          0, 0);

    if(window == nullptr) {
        MR_LOG_ERROR(glfwCreateWindow, "failed create window");
        glfwTerminate();
        return -1;
    }
    glfwContextMgr ctxMgr(window);

    if(!mr::Core::Init(&ctxMgr)) {
        MR_LOG_ERROR(main, "MR Core init failed");
        return -1;
    }

    main_logic(window);

    mr::Core::Shutdown();

    return 0;
}
