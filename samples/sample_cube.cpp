#include "../mr/mr.hpp"

#include <GLFW/glfw3.h>
#include <thread>
#include <fstream>

//
/// Define data
//
const char* vertexShader =
"#version 410 \n"
"#extension GL_ARB_bindless_texture : require \n"
"#extension GL_NV_gpu_shader5 : enable \n"

"in struct mr_vertex_type { \n"
"   layout (location = 0) vec3 pos; \n"
"   layout (location = 1) vec2 texCoord; \n"
"} mr_vertex; \n"

"out vec4 gl_Position; \n"
"out vec3 world_pos; \n"
"out vec2 vtexCoord; \n"

"uniform struct mr_sys_type { \n"
"   float time; \n"
"} mr_sys; \n"

"uniform struct mr_scene_type { \n"
"   mat4 modelMat; \n"
"} mr_scene;\n"

"uniform struct mr_camera_type { \n"
"   mat4 projMat; \n"
"   mat4 viewMat; \n"
"} mr_camera;\n"

"uniform struct mr_material_type { \n"
"   uint64_t diffuseTex; \n"
"} mr_material; \n"

"void main() { \n"
"   vec4 vertex_world_pos = mr_scene.modelMat * vec4(mr_vertex.pos + gl_InstanceID * vec3(2.0, 0.0, 0.0), 1.0); \n"
"   gl_Position = (mr_camera.projMat * mr_camera.viewMat) * vertex_world_pos; \n"
"   world_pos = vertex_world_pos.xyz; \n"
"   vtexCoord = mr_vertex.texCoord; \n"
"} \n"
;

const char* fragmentShader =
"#version 400 \n"
"#extension GL_ARB_bindless_texture : require \n"
"#extension GL_NV_gpu_shader5 : enable \n"

"in vec3 world_pos; \n"
"in vec2 vtexCoord; \n"

"out vec3 mr_fragColor; \n"

"uniform struct mr_sys_type { \n"
"   float time; \n"
"} mr_sys; \n"

"uniform struct mr_scene_type { \n"
"   mat4 modelMat; \n"
"} mr_scene;\n"

"uniform struct mr_camera_type { \n"
"   mat4 projMat; \n"
"   mat4 viewMat; \n"
"} mr_camera;\n"

"uniform struct mr_material_type { \n"
"   uint64_t diffuseTex; \n"
"} mr_material; \n"

"void main() { \n"
"   vec3 light_pos = vec3(500, 200, 500); \n"
"   float light = 1.0; \n"
"   mr_fragColor = texture(sampler2D(mr_material.diffuseTex), vtexCoord).rgb * light; \n"
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
    flags.write = true;

    // Don't initialize buffers with memory this time
    // Will do it async further
    auto buf_vert = Buffer::Create(Memory::Zero(vertexDataMem->GetSize()), flags);
    auto buf_ind = Buffer::Create(Memory::Zero(indexDataMem->GetSize()), flags);
    buf_vert->MakeResident(MR_RESIDENT_READ_ONLY);
    buf_ind->MakeResident(MR_RESIDENT_READ_ONLY);

    // Run write async
    auto bufv_write = buf_vert->WriteAsync(vertexDataMem);
    auto bufi_write = buf_ind->WriteAsync(indexDataMem);
    auto tex_load = TextureData::FromFileAsync("texture.jpg");

    // Define vertex format
    auto vdecl = VertexDecl::Create();

    /// Setup vertex format at runtime and export to "vertex.json"
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
    else     // Indecies can be used, without gpu buffer
        ibuffer = IndexBuffer::Create(indexDataMem, IndexDataType::UShort, indexNum);

    // Create and compile shaders
    auto vshader = Shader::Create(ShaderType::Vertex, vertexShader);
    auto fshader = Shader::Create(ShaderType::Fragment, fragmentShader);

    // Create and link shader program
    auto prog = ShaderProgram::Create({vshader, fshader});

    // Wait till async tasks end
    bufv_write.wait();
    bufi_write.wait();
    tex_load.wait();

    // Test texture
    auto textureData = tex_load.get();
    TextureParams textureParams;
    textureParams.minFilter = TextureMinFilter::LinearMipmapLinear;
    textureParams.magFilter = TextureMagFilter::Linear;

    auto texture = Texture2D::Create(textureParams);
    texture->WriteImage(textureData);
    texture->BuildMipmaps();
    texture->MakeResident();

    // Setup uniforms, from uniform blocks
    UniformCache* uniformCache = UniformCache::Get();
    // model matrix will be changed in Update thread
    uniformCache->Set<glm::mat4, 1>("mr_scene.modelMat", glm::mat4(1.0f));
    uniformCache->Set<glm::mat4, 1>(mr::CameraProjMat, glm::perspective(90.0f, 960.0f / 600.0f, 0.1f, 2000.0f));
    uniformCache->Set<glm::mat4, 1>(mr::CameraViewMat, glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,-1), glm::vec3(0,1,0)));
    uniformCache->Set<uint64_t, 1>(mr::MaterialDiffuseTex, texture->GetResidentHandle());

    // Set window 'background' color
    Draw::SetClearColor(10,10,10,255);

    /// Export/Import VertexBuffer
    /*std::ofstream json_file2("vbuffer.json");
    Json<VertexBufferPtr>::Export(vbuffer, json_file2);
    */
    /*std::ifstream json_file2("vbuffer.json");
    Json<VertexBufferPtr>::Import(vbuffer, json_file2);
    */

    std::vector<MeshPtr> sceneMeshes;
    Mesh::ImportOptions importOptions; importOptions.debugLog = true;
    Mesh::Import("sponza.obj", prog, sceneMeshes, importOptions);

    MR_LOG_T_STD_("Loading time (ms): ", loadTimer.End());

    auto renderBuffers = RenderBuffer::Create({960, 600}, {TextureStorageFormat::RGB8, TextureStorageFormat::DepthComponent24});

    auto framebuffer = FrameBuffer::Create();
    framebuffer->SetColor(renderBuffers[0], 0);
    framebuffer->SetDepth(renderBuffers[1]);
    SetDrawFramebuffer(framebuffer);

    // Test camera
    PerspectiveCamera camera;
    camera.SetPos({0,0,0});
    camera.SetRot({0,0,0});

    double lastTime = glfwGetTime();
    int fps = 0;
    double fpsTimer = 1.0;
    while(!glfwWindowShouldClose(window)) {
        double deltaTime = glfwGetTime() - lastTime;

        const float moveSpeed = ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 500.0f : 2.0f) * deltaTime;
        const float rotateSpeed = 90.0f * deltaTime;

        // Clear screen
        Draw::Clear(ClearFlags::ColorDepth);

        // Draw from buffer with shader
        const uint32_t instances = 1;
        //Draw::Primitive(prog, DrawMode::Triangle, vbuffer, ibuffer, instances);

        for(int i = 0, n = sceneMeshes.size(); i < n; ++i) {
            sceneMeshes[i]->Draw(instances);
        }

        // Simple GLFW camera movement
        glm::vec3 cameraMove(0,0,0), cameraRotate(0,0,0);
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraMove.z += moveSpeed;
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraMove.z -= moveSpeed;
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraMove.x += moveSpeed;
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraMove.x -= moveSpeed;
        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cameraMove.y += moveSpeed;
        if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) cameraMove.y -= moveSpeed;
        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) cameraRotate.y += rotateSpeed;
        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) cameraRotate.y -= rotateSpeed;
        camera.Movement(cameraMove, cameraRotate);

        // Update uniform values
        uniformCache->Set<glm::mat4, 1>(mr::CameraViewMat, camera.GetMat());
        uniformCache->Set<float, 1>("mr_sys.time", (float)lastTime);

        lastTime += deltaTime;

        ++fps;
        fpsTimer -= deltaTime;
        if(fpsTimer <= 0.0) {
            fpsTimer = 1.0;
            std::cout << fps << std::endl;
            fps = 0;
        }

        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);

        framebuffer->ToScreen(FrameBufferBit::ColorDepth, {0,0,960,600}, {0,0,window_width,window_height});

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

    GLFWwindow* window = glfwCreateWindow(960, 600,
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
