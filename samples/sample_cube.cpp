#include "../mr/mr.hpp"
#include "../mr/pre/glew.hpp"

#include <GLFW/glfw3.h>

//
/// Define data
//
const char* vertexShader =
"#version 400 \n"
"layout (location = 0) in vec3 pos; \n"
"layout (location = 1) in vec4 color; \n"
"out vec4 gl_Position; \n"
"out vec4 vcolor; \n"
"uniform Mat { \n"
"   mat4 proj; \n"
"   mat4 view; \n"
"   mat4 model; \n"
"}; \n"
"void main() { \n"
"   gl_Position = ((proj * view * model) * vec4(pos, 1.0)); \n"
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

//
/// Work with MorglodsRender
//
void main_logic(GLFWwindow* window) {
    using namespace mr;

    // Print info about current gpu and gl
    Info::Print();

    // Measure loading time
    Timer<HighresClockT, MilliTimeT> loadTimer;
    loadTimer.Start();

    // Prepare memory
    const auto vertexDataMem = Memory::Ref(vertexData, sizeof(Vertex) * 3);
    const auto uboDataMem = Memory::Ref(uboData, sizeof(glm::mat4) * 3);

    // Create buffer and map it
    Buffer::CreationFlags flags;
    flags.map_after_creation = true;

    // Don't initialize buffer with memory this time
    // Will do it async further
    auto buf = Buffer::Create(Memory::Zero(vertexDataMem->GetSize()), flags);

    // Mark readable for shader parameters buffer
    flags.read = true;
    auto ubo = Buffer::Create(Memory::Zero(uboDataMem->GetSize()), flags);

    // Run write async
    auto buf_write = buf->Write(vertexDataMem);
    auto ubo_write = ubo->Write(uboDataMem);

    // Define vertex format
    auto vdecl = VertexDecl::Create();
    auto vdef = vdecl->Begin();
    vdef.Pos()
        .Color()
        .End();

    // Create vertex buffer { buffer + format }
    auto vbuffer = VertexBuffer::Create(buf, vdecl, 3);

    // Create and compile shaders
    auto vshader = Shader::Create(ShaderType::Vertex, std::string(vertexShader));
    auto fshader = Shader::Create(ShaderType::Fragment, std::string(fragmentShader));

    // Create and link shader program
    auto prog = ShaderProgram::Create({vshader, fshader});

    // Set parameters buffer for shaders
    prog->UniformBuffer("Mat", ubo, 0);

    // Get mapped memory ("direct" access to parameters buffer)
    auto ubo_mat = (glm::mat4*) ubo->GetMapState().mem;

    // Set draw color
    Draw::ClearColor(125,125,125,255);

    // Wait till async tasks end
    buf_write.wait();
    ubo_write.wait();

    MR_LOG_T_STD_("Loading time (ms): ", loadTimer.End());

    float a = 0.0f;
    while(!glfwWindowShouldClose(window)) {
        // Clear screen
        Draw::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw from buffer with shader
        Draw::Primitive(prog, DrawMode::Triangle, vbuffer, nullptr);

        // Update shader parameters
        ubo_mat[2] = glm::rotate(a, glm::vec3(1,1,1));
        a += 0.0001f;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main() {
    /// Init context with GLFW3.
    // May be used any context manager.
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
    glfwMakeContextCurrent(window);

    /// Init MorglodsRender.
    // Context should be set.
    if(!mr::Core::Init()) {
        MR_LOG_ERROR(main, "MR Core init failed");
        return -1;
    }

    main_logic(window);

    /// Free all resources and shutdown MorglodsRender.
    mr::Core::Shutdown();

    return 0;
}
