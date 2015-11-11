#include "../mr/mr.hpp"
#include "../mr/pre/glew.hpp"

#include "sample_doom_geom.hpp"
#include "sample_doom_texture.hpp"
#include "sample_doom_shader.hpp"
#include "sample_doom_map.hpp"

#include <GLFW/glfw3.h>
#include <thread>
#include <fstream>

void main_logic(GLFWwindow* window) {
    using namespace mr;

    // Print info about current gpu and gl
    Info::Print();

    // Measure loading time
    Timer<HighresClockT, MilliTimeT> loadTimer;
    loadTimer.Start();

    // Set 'background' color
    Draw::SetClearColor(10,10,10,255);

    // Load
    Pre_InitGeom();
    InitTexture();
    InitMap();
    InitShader();
    Post_InitGeom();


    mr::BufferPtr map_blocks_model = CreateModelData(wall_texture->GetResidentHandle(), map_blocks_ubo->GetResidentHandle()),
                  map_planes_model = CreateModelData(floor_texture->GetResidentHandle(), map_planes_ubo->GetResidentHandle());

    MR_LOG_T_STD_("Loading time (ms): ", loadTimer.End());

    // Setup camera
    PerspectiveCamera camera;
    camera.SetPos({0,0.2f,0});
    camera.SetRot({0,0,0});
    ubo_data_ptr->proj = glm::perspective(90.0f, 800.0f / 600.0f, 0.1f, 10.0f);
    ubo_data_ptr->view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,-1), glm::vec3(0,1,0));
    ubo_data_ptr->model = glm::mat4(1.0f);

    double lastTime = glfwGetTime(), timeDelta = 0.0;
    while(!glfwWindowShouldClose(window)) {
        const double newTime = glfwGetTime();
        timeDelta = newTime - lastTime;
        lastTime = newTime;

        const float moveSpeed = 2.0f * timeDelta;
        const float rotSpeed = 200.0f * timeDelta;

        // Clear screen
        Draw::Clear(ClearFlags::ColorDepth);

        // Draw from buffer with shader
        shaderProgram->UniformBuffer("ModelData", map_blocks_model, 1);
        Draw::Primitive(shaderProgram, DrawMode::Triangle, box_vertexBuffer, box_indexBuffer, map_blocks_num);

        shaderProgram->UniformBuffer("ModelData", map_planes_model, 1);
        Draw::Primitive(shaderProgram, DrawMode::Triangle, plane_vertexBuffer, plane_indexBuffer, map_planes_num);

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.MoveForward(moveSpeed);
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.MoveForward(-moveSpeed);
        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.RotateY(rotSpeed);
        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.RotateY(-rotSpeed);
        ubo_data_ptr->view = camera.GetMat();

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

