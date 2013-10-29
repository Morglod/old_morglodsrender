#include "MorglodsRender.h"

unsigned int LastErrorCode = 0;

GLFWwindow* MR::Init(const char* window_name, int width, int height){
    if(!glfwInit()){
        LastErrorCode = 1;
        return NULL;
    }

    GLFWwindow* window = glfwCreateWindow(width, height, window_name, NULL, NULL);

    if(!window){
        LastErrorCode = 2;
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);

    GLenum result = glewInit();

    if(result != GLEW_OK){
        LastErrorCode = 3;
        return NULL;
    }

    glEnable(GL_TEXTURE_2D);

    return window;
}

void MR::Shutdown(){
    MR::TextureManager::Instance()->RemoveAll();
    //MR::MeshManager::Instance()->RemoveAll();
    glfwTerminate();
}
/*
void MR::Message(unsigned int code){
    LastErrorCode = code;
}

const char* MR::MessageText(unsigned int code){
    if(code == 0) return "NULL";
    else if(code == 1) return "ERROR: glfwInit() in MR::Init";
    else if(code == 2) return "ERROR: glfwCreateWindow() in MR::Init";
    else if(code == 3) return "ERROR: glewInit() in MR::Init";
    else if(code == 4) return "OK";
    else if(code == 5) return "DEBUG MESSAGE";
    else if(code == 6) return "DEBUG MESSAGE2";
    return "ERROR: bad code";
}

unsigned int MR::GetLastErrorCode(){return LastErrorCode;}
*/
