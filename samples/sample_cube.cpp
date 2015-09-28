#include "../mr/mr.hpp"

#include <GLFW/glfw3.h>
#include <thread>
#include <iostream>

const char* vertexShader =
"#version 400 \n"
"layout (location = 0) in vec3 pos; \n"
"layout (location = 1) in vec4 color; \n"
"out vec4 vcolor; \n"
"uniform mat4 proj; \n"
"uniform mat4 view; \n"
"uniform mat4 model; \n"
"void main() { \n"
"   gl_Position.xyz = ((proj * view * model) * vec4(pos, 1.0)).xyz; \n"
"   gl_Position.w = 1.0; \n"
"   vcolor = color; \n"
"} \n"
;

const char* fragmentShader =
"#version 400 \n"
"in vec4 vcolor; \n"
"out vec3 fragColor; \n"
"void main() { \n"
"   fragColor = vcolor.xyz; \n"
"} \n"
;

void main_logic(GLFWwindow* window) {
    using namespace mr;

    Core::Exec([](void*){ Info::PrintInfo(); }).wait();

    struct Vertex {
        float xyz[3];
        uint8_t color[4];
    };

    static Vertex vertexData[] = {
        {0.0f,  0.5f,  0.0f, 255, 0, 0, 255},
        {0.5f, -0.5f,  0.0f, 0, 255, 0, 255},
        {-0.5f, -0.5f, 0.0f, 0, 0, 255, 0}
    };

    const auto vertexDataMem = Memory::Ref(vertexData, sizeof(Vertex) * 3);

    Buffer::CreationCmd cmd;
    cmd.map_after_creation = true;
    auto buf = Buffer::Create(vertexDataMem, cmd);

    auto vdecl = VertexDecl::Create();
    auto vdef = vdecl->Begin();
    vdef.Pos()
        .Color()
        .End();

    auto vbuffer = VertexBuffer::Create(buf.get(), vdecl, 3);

    auto vshader = Shader::Create(ShaderType::Vertex, std::string(vertexShader));
    auto fshader = Shader::Create(ShaderType::Fragment, std::string(fragmentShader));
    auto prog = ShaderProgram::Create({vshader.get(), fshader.get()}).get();

    prog->UniformMat4("model", glm::mat4(1.0f));
    prog->UniformMat4("view", glm::mat4(1.0f));
    prog->UniformMat4("proj", glm::mat4(1.0f));

    Draw::ClearColor(125,125,125,255);
    while(!glfwWindowShouldClose(window)) {
        Draw::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Draw::Primitive(prog, DrawMode::Triangle, vbuffer, nullptr);

        Core::Exec([](void* wnd){ glfwSwapBuffers((GLFWwindow*)wnd); }, window).wait();
        glfwPollEvents();
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
