#include "../mr/mr.hpp"

#include <GLFW/glfw3.h>
#include <thread>
#include <iostream>

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

void main_logic();

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

    main_logic();

    mr::Core::Shutdown();

    return 0;
}















struct Vertex {
    char word[3];
};

void main_logic() {
    using namespace mr;

    Core::Exec([](void*){ Info::PrintInfo(); }).wait();

    static Vertex vertexData[] = {
        { 'a', 'b', '\0' },
        { '1', '2', '\0' }
    };
    const auto vertexDataMem = Memory::Ref(vertexData, sizeof(vertexData));

    Buffer::CreationCmd cmd;
    cmd.map_after_creation = true;
    auto buf = Buffer::Create(vertexDataMem, cmd).get();

    Vertex* mem = static_cast<Vertex*>(buf->GetMappedMem().mem);
    for(int i = 0; i < 2; ++i) {
        MR_LOG(std::string(mem[i].word));
    }

    buf->UnMap().wait();
}
