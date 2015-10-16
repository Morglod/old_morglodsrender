#include "../mr/mr.hpp"

#include "../mr/pre/glew.hpp"
#include <GLFW/glfw3.h>
#include <thread>
#include <iostream>


const char* vertexShader =
"#version 400 \n"
"layout (location = 0) in vec3 pos; \n"
"layout (location = 1) in vec4 color; \n"
"out vec4 vcolor; \n"
"uniform Mat { \n"
"   mat4 proj; \n"
"   mat4 view; \n"
"   mat4 model; \n"
"}; \n"
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

    VertexBufferPtr vbuffer = nullptr;
    ShaderProgramPtr prog = nullptr;
    glm::mat4* ubo_mat = nullptr;

    Core::Exec([window, &vbuffer, &prog, &ubo_mat](void*) -> uint8_t {
        Info::PrintInfo();

        struct Vertex {
            float xyz[3];
            uint8_t color[4];
        };

        static Vertex vertexData[] = {
            {0.0f,  0.5f,  0.0f, 255,   0,   0, 255},
            {0.5f, -0.5f,  0.0f,   0, 255,   0, 255},
            {-0.5f,-0.5f,  0.0f,   0,   0, 255,   0}
        };

        static glm::mat4 uboData[] = {
            glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)
        };

        const auto vertexDataMem = Memory::Ref(vertexData, sizeof(Vertex) * 3);
        const auto uboDataMem = Memory::Ref(uboData, sizeof(glm::mat4) * 3);

        Buffer::CreationFlags flags;
        flags.map_after_creation = true;
        auto buf = Buffer::Create(vertexDataMem, flags);
        auto ubo = Buffer::Create(uboDataMem, flags).get();

        auto vdecl = VertexDecl::Create();
        auto vdef = vdecl->Begin();
        vdef.Pos()
            .Color()
            .End();

        vbuffer = VertexBuffer::Create(buf.get(), vdecl, 3);

        auto vshader = Shader::Create(ShaderType::Vertex, std::string(vertexShader));
        auto fshader = Shader::Create(ShaderType::Fragment, std::string(fragmentShader));
        prog = ShaderProgram::Create({vshader.get(), fshader.get()}).get();

        prog->UniformBuffer("Mat", ubo, 0);
        ubo_mat = (glm::mat4*) ubo->GetMapState().mem;

        Draw::ClearColor(125,125,125,255).wait();

    float a = 0.0f;
    while(!glfwWindowShouldClose(window)) {
        //Core::Exec([&a, &vbuffer, &prog, &ubo_mat](void*){
            //Draw::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //Draw::Primitive(prog, DrawMode::Triangle, vbuffer, nullptr);
            glUseProgram(prog->_id);
            VertexBuffer::_Bind(vbuffer.get(), 0, 0);
            const uint32_t baseInstance = 0, instancesNum = 1, baseVertex = 0, baseIndex = 0;
            glDrawArraysInstancedBaseInstance((uint32_t)DrawMode::Triangle, baseVertex, vbuffer->_num, instancesNum, baseInstance);

            ubo_mat[2] = glm::rotate(a, glm::vec3(1,1,1));
            a += 0.0001f;

     //       Core::Swap()/*;
     //   })*/.wait();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
    }).wait();
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

    void Swap() override {
        glfwSwapBuffers((GLFWwindow*)mainWindow);
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
