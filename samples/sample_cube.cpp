#include "../mr/mr.hpp"

#include <GLFW/glfw3.h>
#include <thread>
#include <fstream>

//
/// Define data
//
const char* vertexShader =
"#version 400 \n"
"#extension GL_ARB_bindless_texture : require \n"
"#extension GL_NV_gpu_shader5 : enable \n"
"layout (location = 0) in vec3 pos; \n"
"layout (location = 1) in vec4 color; \n"
"layout (location = 2) in float color2; \n"
"layout (location = 3) in vec2 texCoord; \n"
"out vec4 gl_Position; \n"
"out vec4 vcolor; \n"
"out vec2 vtexCoord; \n"
"{{SYS_UNIFORM}} \n"
"uniform UBOData { \n"
"   mat4 model; \n"
"   uint64_t tex; \n"
"}; \n"
"void main() { \n"
"   gl_Position = ((mr_sys.proj * mr_sys.view * model) * vec4(pos + gl_InstanceID * vec3(2.0, 0.0, 0.0), 1.0)); \n"
"   vcolor = color * color2; \n"
"   vtexCoord = texCoord; \n"
"} \n"
;

const char* fragmentShader =
"#version 400 \n"
"#extension GL_ARB_bindless_texture : require \n"
"#extension GL_NV_gpu_shader5 : enable \n"
"in vec4 vcolor; \n"
"in vec2 vtexCoord; \n"
"out vec3 fragColor; \n"
"{{SYS_UNIFORM}} \n"
"uniform UBOData { \n"
"   mat4 model; \n"
"   uint64_t tex; \n"
"}; \n"
"void main() { \n"
"   fragColor = ((texture(sampler2D(tex), vtexCoord).rgb + vcolor.xyz) * 0.5) * ((1.0 + sin(mr_sys.time)) / 1.5) + vec3(0.1, 0.1, 0.1); \n"
"} \n"
;

struct Vertex {
    float xyz[3];
    uint32_t color_argb;
    float color2;
    float texCoord[2];
};

const size_t vertexNum = 8;
static Vertex vertexData[vertexNum] = {
    {-1.0f,  1.0f,  1.0f, 0xff000000, 0.1f, 0,1 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff, 0.2f, 1,1 },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00, 0.3f, 0,0 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff, 0.1f, 1,0 },
	{-1.0f,  1.0f, -1.0f, 0xffff0000, 0.2f, 0,1 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff, 0.3f, 1,1 },
	{-1.0f, -1.0f, -1.0f, 0xffffff00, 0.1f, 0,0 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff, 0.2f, 1,0 },
};

const size_t indexNum = 36;
static uint16_t indexData[indexNum] = {
	0, 1, 2, // 0
	1, 3, 2,
	4, 6, 5, // 2
	5, 6, 7,
	0, 2, 4, // 4
	4, 2, 6,
	1, 5, 3, // 6
	5, 7, 3,
	0, 4, 1, // 8
	4, 5, 1,
	2, 3, 6, // 10
	6, 3, 7,
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
    const auto vertexDataMem = Memory::Ref(vertexData, sizeof(Vertex) * vertexNum);
    const auto indexDataMem = Memory::Ref(indexData, sizeof(uint16_t) * indexNum);

    // Create buffer and map it
    Buffer::CreationFlags flags;
    flags.map_after_creation = true;

    // Don't initialize buffers with memory this time
    // Will do it async further
    auto buf_vert = Buffer::Create(Memory::Zero(vertexDataMem->GetSize()), flags);
    auto buf_ind = Buffer::Create(Memory::Zero(indexDataMem->GetSize()), flags);
    buf_vert->MakeResident(MR_RESIDENT_READ_ONLY);
    buf_ind->MakeResident(MR_RESIDENT_READ_ONLY);

    // Run write async
    auto bufv_write = buf_vert->WriteAsync(vertexDataMem);
    auto bufi_write = buf_ind->WriteAsync(indexDataMem);

    // Define vertex format
    auto vdecl = VertexDecl::Create();

    /// At runtime and export to "vertex.json"
    auto vdef = vdecl->Begin();
    vdef.Pos()
        .Color()
        .Custom(DataType::Float, 1, 0, true)
        .Custom(DataType::Float, 2, 0, true)
        .End();

    std::ofstream json_file("vertex.json");
    Json<VertexDeclPtr>::Export(vdecl, json_file);

    /// Import from "vertex.json"
    /*std::ifstream json_file("vertex.json");
    Json<VertexDeclPtr>::Import(vdecl, json_file);
    */

    // Create vertex and index buffers { buffer + format }
    auto vbuffer = VertexBuffer::Create(buf_vert, vdecl, vertexNum);
    IndexBufferPtr ibuffer = nullptr;
    if(true) // Use buffer for indecies
        ibuffer = IndexBuffer::Create(buf_ind, IndexDataType::UShort, indexNum);
    else
        ibuffer = IndexBuffer::Create(indexDataMem, IndexDataType::UShort, indexNum);

    // Create and compile shaders
    auto vshader = Shader::Create(ShaderType::Vertex, ReplaceString(std::string(vertexShader), "{{SYS_UNIFORM}}", SysUniformStr));
    auto fshader = Shader::Create(ShaderType::Fragment, ReplaceString(std::string(fragmentShader), "{{SYS_UNIFORM}}", SysUniformStr));

    auto uboDecl = UniformBufferDecl::Create();
    auto uboDeclEdit = uboDecl->Begin();
    uboDeclEdit.Float(3)
    .End();

    // Create and link shader program
    auto prog = ShaderProgram::Create({vshader, fshader});

    // Test material
    MaterialShaderPtr material = MaterialShader::Create(prog);

    // Set parameters buffer for shaders
    UniformBufferPtr ubo_data_uniform = UniformBuffer::Create(UniformBufferDesc::Create(prog, "UBOData"));
    ubo_data_uniform->Binding(0);

    // Get mapped memory ("direct" access to parameters buffer)
    auto sysUBO = (SysUniformData*)MaterialShader::GetSystemUniformBuffer()->GetMapState().mem;

    // Set 'background' color
    Draw::SetClearColor(10,10,10,255);

    // Wait till async tasks end
    bufv_write.wait();
    bufi_write.wait();

    /// Export VertexBuffer
    /*std::ifstream json_file2("vbuffer.json");
    Json<VertexBufferPtr>::Import(vbuffer, json_file2);
    */

    // Setup shader parameters
    sysUBO->proj = glm::perspective(90.0f, 800.0f / 600.0f, 0.1f, 10.0f);
    sysUBO->view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,-1), glm::vec3(0,1,0));
    // sysUBO->model will be changed in Update thread

    // Test texture
    auto textureData = TextureData::FromFile("house.png");

    TextureParams textureParams;
    textureParams.minFilter = TextureMinFilter::LinearMipmapLinear;
    textureParams.magFilter = TextureMagFilter::Linear;

    auto texture = Texture2D::Create(textureParams);
    texture->Storage();
    texture->WriteImage(textureData);
    texture->BuildMipmaps();
    texture->MakeResident();
    ubo_data_uniform->As<uint64_t>("tex") = texture->GetResidentHandle();

    MR_LOG_T_STD_("Loading time (ms): ", loadTimer.End());

    // Test camera
    PerspectiveCamera camera;
    camera.SetPos({0,0,0});
    camera.SetRot({0,0,0});

    // Update thread example
    bool update_thread_working = true;
    auto update_thread = std::thread([&update_thread_working](glm::mat4* modelMat) {
                                        float a = 0.0f;
                                        while(update_thread_working) {
                                            *modelMat = glm::rotate(a, glm::vec3(1,1,1));
                                            a += 0.01f;
                                            std::this_thread::sleep_for(std::chrono::milliseconds(16));
                                        }
                                     }, ubo_data_uniform->AsPtr<glm::mat4>("model"));

    while(!glfwWindowShouldClose(window)) {
        // Clear screen
        Draw::Clear(ClearFlags::ColorDepth);

        // Draw from buffer with shader
        const uint32_t instances = 10;
        Draw::Primitive(prog, DrawMode::Triangle, vbuffer, ibuffer, instances);

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.MoveForward(0.001f);
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.MoveForward(-0.001f);
        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.RotateY(0.1f);
        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.RotateY(-0.1f);
        sysUBO->view = camera.GetMat();
        sysUBO->time = (float)glfwGetTime();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    update_thread_working = false;
    update_thread.join();
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
                                          nullptr, nullptr);

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
