#include <MorglodsRender.h>

#define timeType float

int main()
{
    GLFWwindow* window = MR::Init("Test window", 640, 480);

    std::cout << "Machine info:" <<
    "\nVersion: " << MR::MachineInfo::version_string() <<
    "\nGLSL: " << MR::MachineInfo::version_glsl() <<
    "\nOpenGL: " << MR::MachineInfo::gl_version_major() << " " << MR::MachineInfo::gl_version_minor() <<
    "\nGPU: " << MR::MachineInfo::gpu_name() << " from " << MR::MachineInfo::gpu_vendor_string() <<
    "\nMem Total(kb): " << MR::MachineInfo::total_memory_kb() << " Current(kb): " << MR::MachineInfo::current_memory_kb() << "\n";

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    const float z = -0.5f;
    const int garray_size = 2*3*4;
    float garray[garray_size]{
        //v1
        -0.5f, -0.5f, z,
        1.0f, 0.0f, 0.0f,
        //v2
        -0.5f, 0.5f, z,
        0.0f, 1.0f, 0.0f,
        //v4
        0.5f, 0.5f, z,
        1.0f, 1.0f, 1.0f,
        //v3
        0.5f, -0.5f, z,
        0.0f, 0.0f, 1.0f
    };

    const int iarray_size = 6;
    unsigned int iarray[6]{
        0,1,2,
        0,3,2
    };

    MR::VertexDeclarationType vdtypes[2]{
        MR::VertexDeclarationType(MR::VertexDeclarationTypesEnum::VDTE_POSITION, 3, 0),
        MR::VertexDeclarationType(MR::VertexDeclarationTypesEnum::VDTE_COLOR, 3, (void*)(sizeof(float)*3))
    };

    MR::VertexDeclaration vdecl(&vdtypes[0], 2, GL_FLOAT);
    MR::IndexDeclaration idecl(GL_UNSIGNED_INT);

    MR::GeometryBuffer* geom = new MR::GeometryBuffer(&vdecl, &idecl, &garray[0], sizeof(float)*garray_size, &iarray[0], sizeof(unsigned int)*iarray_size, garray_size/6, iarray_size, GL_STATIC_DRAW, GL_STATIC_DRAW, GL_TRIANGLES);

    gluPerspective(90, 640/480, 0.1f, 10.0f);

    geom->Bind();

    unsigned short lfps = 0;
    unsigned short curFps = 0;
    while(true/*!glfwWindowShouldClose(window)*/){
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        geom->Draw();

        glfwPollEvents();
        glfwSwapBuffers(window);

        curFps = MR::FPS<float>();
        if(curFps != lfps){
            std::cout << "FPS " << curFps << '\n';
            lfps = curFps;
        }
    }

    geom->Unbind();

    MR::Shutdown();

    return 0;
}
