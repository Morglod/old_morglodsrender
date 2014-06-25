#include "MachineInfo.hpp"
#include "Utils/Log.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#include <windows.h>

// GL_NVX_gpu_memory_info
#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX     0x9047
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX     0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX   0x9049

// ATI_meminfo
#define VBO_FREE_MEMORY_ATI             0x87FB
#define TEXTURE_FREE_MEMORY_ATI         0x87FC
#define RENDERBUFFER_FREE_MEMORY_ATI    0x87FD

// AMD_gpu_association
#define WGL_GPU_VENDOR_AMD                 0x1F00
#define WGL_GPU_RENDERER_STRING_AMD        0x1F01
#define WGL_GPU_OPENGL_VERSION_STRING_AMD  0x1F02
#define WGL_GPU_FASTEST_TARGET_GPUS_AMD    0x21A2
#define WGL_GPU_RAM_AMD                    0x21A3
#define WGL_GPU_CLOCK_AMD                  0x21A4
#define WGL_GPU_NUM_PIPES_AMD              0x21A5
#define WGL_GPU_NUM_SIMD_AMD               0x21A6
#define WGL_GPU_NUM_RB_AMD                 0x21A7
#define WGL_GPU_NUM_SPI_AMD                0x21A8

int MR::MachineInfo::gl_version_major() {
    int outv = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &outv);
    return outv;
}

int MR::MachineInfo::gl_version_minor() {
    int outv = 0;
    glGetIntegerv(GL_MINOR_VERSION, &outv);
    return outv;
}

std::string MR::MachineInfo::gl_version_string() {
    static std::string outv = (const char*)glGetString(GL_VERSION);
    return outv;
}

MR::MachineInfo::GLVersion MR::MachineInfo::gl_version() {
    static int gl_major = gl_version_major();
    static int gl_minor = gl_version_minor();
    static GLVersion ver = GLVersion::VUnknown;

    if(ver != GLVersion::VUnknown) return ver;

    if( (gl_major <= MINIMAL_GL_VERSION_MAJOR) && (gl_minor <= MINIMAL_GL_VERSION_MINOR) ) {
        ver = GLVersion::VNotSupported;
        return ver;
    }
    if(gl_major == 3) {
        if(gl_minor == 2) {
            ver = GLVersion::V3_2;
            return ver;
        } else if(gl_minor == 3) {
            ver = GLVersion::V3_3;
            return ver;
        } else {
            ver = GLVersion::V3_x;
            return ver;
        }
    } else if(gl_major == 4) {
        if(gl_minor == 0) {
            ver = GLVersion::V4_0;
            return ver;
        } else if(gl_minor == 2) {
            ver = GLVersion::V4_2;
            return ver;
        } else if(gl_minor == 3) {
            ver = GLVersion::V4_3;
            return ver;
        } else if(gl_minor == 4) {
            ver = GLVersion::V4_4;
            return ver;
        } else {
            ver = GLVersion::V4_x;
            return ver;
        }
    }

    ver = GLVersion::Vx_x;
    return ver;
}

std::string MR::MachineInfo::gpu_vendor_string() {
    static std::string outv = (const char*)glGetString(GL_VENDOR);
    return outv;
}

MR::MachineInfo::GPUVendor MR::MachineInfo::gpu_vendor() {
    static std::string v = gpu_vendor_string();
    if(v == "NVIDIA Corporation") return GPUVendor::Nvidia;
    else if(v == "ATI Technologies") return GPUVendor::ATI;
    else if(v == "INTEL") return GPUVendor::Intel;
    else if(v == "Microsoft") return GPUVendor::Microsoft_DirectX_Wrapper;
    else return GPUVendor::Other;
}

std::string MR::MachineInfo::gpu_name() {
    static std::string outv = (const char*)glGetString(GL_RENDERER);
    return outv;
}

std::string MR::MachineInfo::gl_extensions_list() {
    static std::string outv = (const char*)glGetString(GL_EXTENSIONS);
    return outv;
}

std::string MR::MachineInfo::gl_version_glsl() {
    static std::string outv = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    return outv;
}

int MR::MachineInfo::total_memory_kb() {
    GLint total_mem_kb = 0;
    if(gpu_vendor() == GPUVendor::Nvidia) glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
    /*else if(gpu_vendor() == GPUVendor::ATI) {
        unsigned int n = wglGetGPUIDsAMD(0, 0);
        unsigned int *ids = new unsigned int[n];
        size_t total_mem_mb = 0;
        wglGetGPUIDsAMD(n, ids);
        wglGetGPUInfoAMD(ids[0],
          WGL_GPU_RAM_AMD,
          GL_UNSIGNED_INT,
          sizeof(size_t),
          &total_mem_mb);
        total_mem_kb = total_mem_mb / 1024;
    }*/
    else {
        //COPYPASTA
        DWORD	i[5] = { 0, 0, 0x27, 0, 0 };
        DWORD	o[5] = { 0, 0, 0, 0, 0 };
#ifdef UNICODE
        HDC hdc = CreateDC(L"DISPLAY", 0, 0, 0);
#else
        HDC hdc = CreateDC("DISPLAY", 0, 0, 0);
#endif
        if (hdc == NULL) {
            MR::Log::LogString("Failed MachineInfo::total_memory_kb. Failed CreateDC", MR_LOG_LEVEL_ERROR);
            return 0;
        }

        int s = ExtEscape(hdc, 0x7032, 0x14, (LPCSTR)i, 0x14, (LPSTR)o);

        DeleteDC(hdc);

        if (s <= 0) {
            MR::Log::LogString("Failed MachineInfo::total_memory_kb. Bad Escape code", MR_LOG_LEVEL_ERROR);
            return 0;
        }

        return (o[3] * 1048576)*1024;
    }
    return total_mem_kb;
}

int MR::MachineInfo::current_memory_kb() {
    GLint cur_avail_mem_kb = 0;
    if(gpu_vendor() == GPUVendor::Nvidia) glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);
    else if(gpu_vendor() == GPUVendor::ATI){
        int free_mem[4] = {0};
        glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &free_mem[0]);
        cur_avail_mem_kb = free_mem[0];
    }
    return cur_avail_mem_kb;
}

bool MR::MachineInfo::FeatureNV_GPUPTR(){
    static bool support = (gpu_vendor() == MR::MachineInfo::GPUVendor::Nvidia) && (__glewGetBufferParameterui64vNV);
    return false;
}

bool MR::MachineInfo::IsDirectStateAccessSupported(){
    static bool state = (__glewNamedBufferDataEXT);
    return true;
}

bool MR::MachineInfo::CatchError(std::string* errorOutput, int * glCode){
    GLenum er = glGetError();
    if(glCode) *glCode = er;

    if(errorOutput) {
        switch(er){
        case GL_INVALID_ENUM:
            *errorOutput = "Invalid enumiration";
            return true;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            *errorOutput = "Invalid framebuffer operation";
            return true;
            break;
        case GL_INVALID_INDEX:
            *errorOutput = "Invalid index";
            return true;
            break;
        case GL_INVALID_OPERATION:
            *errorOutput = "Invalid operation";
            return true;
            break;
        case GL_INVALID_VALUE:
            *errorOutput = "Invalid value";
            return true;
            break;
        case GL_OUT_OF_MEMORY:
            MR::Log::LogString("Out of memory", MR_LOG_LEVEL_ERROR);
            *errorOutput = "Out of memory";
            return true;
            break;
        case GL_STACK_UNDERFLOW:
            *errorOutput = "Stack underflow";
            return true;
            break;
        case GL_STACK_OVERFLOW:
            *errorOutput = "Stack overflow";
            return true;
            break;
        }
    }

    return false;
}

void MR::MachineInfo::ClearError(){
    glGetError();
}

int MR::MachineInfo::MaxTextureSize(){
    static int s = -10;
    if(s == -10){
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &s);
    }
    return s;
}

int MR::MachineInfo::MaxTextureUnits(){
    static int s = 0;
    if(s == 0){
        glGetIntegerv(GL_MAX_TEXTURE_UNITS, &s);
    }
    return s;
}
