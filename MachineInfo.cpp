#include "MachineInfo.hpp"
#include "Utils/Log.hpp"
#include "Context.hpp"

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

typedef unsigned int (*PROC_wglGetGPUIDsAMD)(unsigned int maxCount, unsigned int * ids);
typedef int (*PROC_wglGetGPUInfoAMD)(unsigned int id, int property, GLenum dataType, unsigned int size, void * data);

PROC_wglGetGPUIDsAMD wglGetGPUIDsAMD = 0;
PROC_wglGetGPUInfoAMD wglGetGPUInfoAMD = 0;

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
    else if(v == "ATI Technologies" || v == "ATI Technologies Inc.") return GPUVendor::ATI;
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

unsigned int MR::MachineInfo::total_memory_kb() {
    unsigned int total_mem_kb = 0;
    if(gpu_vendor() == GPUVendor::Nvidia) glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, (int*)&total_mem_kb);
    else if(gpu_vendor() == GPUVendor::ATI) {
        //glGetIntegerv(VBO_FREE_MEMORY_ATI, (int*)&total_mem_kb);
        /*if(wglGetGPUIDsAMD == 0) {
            wglGetGPUIDsAMD = (PROC_wglGetGPUIDsAMD)wglGetProcAddress("wglGetGPUIDsAMD");
            wglGetGPUInfoAMD = (PROC_wglGetGPUInfoAMD)wglGetProcAddress("wglGetGPUInfoAMD");
        }
        if(wglGetGPUIDsAMD == 0) {
            MR::Log::LogString("Failed wglGetProcAddress(\"wglGetGPUIDsAMD\") at MR::MachineInfo::total_memory_kb().", MR_LOG_LEVEL_ERROR);
            return 0;
        }
        unsigned int uNoOfGPUs = wglGetGPUIDsAMD(0,0);
        unsigned int* uGPUIDs = new unsigned int[uNoOfGPUs];
        wglGetGPUIDsAMD(uNoOfGPUs,uGPUIDs);

        wglGetGPUInfoAMD(uGPUIDs[0], WGL_GPU_RAM_AMD, GL_UNSIGNED_INT, sizeof(unsigned int), &total_mem_kb);
        total_mem_kb /= 1024;*/
        MR::Log::LogString("Failed MR::MachineInfo::total_memory_kb(). AMD is glitchy shit. Sorry", MR_LOG_LEVEL_ERROR);
    }
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

unsigned int  MR::MachineInfo::current_memory_kb() {
    unsigned int cur_avail_mem_kb = 0;
    if(gpu_vendor() == GPUVendor::Nvidia) glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, (int*)&cur_avail_mem_kb);
    else if(gpu_vendor() == GPUVendor::ATI){
        glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, (int*)&cur_avail_mem_kb);
    }
    return cur_avail_mem_kb;
}

void MR::MachineInfo::PrintInfo() {
    MR::Log::LogString(
            std::string("Machine info:") +
            std::string("\nVersion: ") + MR::MachineInfo::gl_version_string() +
            std::string("\nGLSL: ") + MR::MachineInfo::gl_version_glsl() +
            std::string("\nOpenGL: ") + std::to_string(MR::MachineInfo::gl_version_major()) + std::string(" ") + std::to_string(MR::MachineInfo::gl_version_minor()) +
            std::string("\nGPU: ") + MR::MachineInfo::gpu_name() + std::string(" from ") + MR::MachineInfo::gpu_vendor_string() +
            std::string("\nMem Total(kb): ") + std::to_string(MR::MachineInfo::total_memory_kb()) + std::string(" Current free (kb): ") + std::to_string(MR::MachineInfo::current_memory_kb()) + "\n\n"
        , MR_LOG_LEVEL_INFO);

        MR::Log::LogString("\nNvidia VBUM: " + std::to_string(MR::MachineInfo::IsNVVBUMSupported()));
        MR::Log::LogString("Direct state access: " + std::to_string(MR::MachineInfo::IsDirectStateAccessSupported()));
    MR::MachineInfo::ClearError();
}

const bool MR::MachineInfo::IsNVVBUMSupported(){
    static bool support = GLEW_NV_vertex_buffer_unified_memory;
    return support;
}

const bool MR::MachineInfo::IsIndirectDrawSupported() {
    static bool support = GLEW_ARB_draw_indirect;//(__glewDrawArraysIndirect);
    return support;
}

const bool MR::MachineInfo::IndirectDraw_UseGPUBuffer() {
    return MR::MachineInfo::IsIndirectDrawSupported();
}

const bool MR::MachineInfo::IsDirectStateAccessSupported(){
    static bool state = GLEW_EXT_direct_state_access;//(__glewNamedBufferDataEXT);
    return state;
}

const bool MR::MachineInfo::IsVertexAttribBindingSupported() {
    static bool state = GLEW_ARB_vertex_attrib_binding;//ctx->ExtensionSupported("ARB_vertex_attrib_binding");
    return state;
}

const bool MR::MachineInfo::IsBufferStorageSupported() {
    static bool state = (__glewBufferStorage);
    return state;
}

int MR::MachineInfo::GetGeometryStreamsNum() {
    static int num = -10;
    if(num == -10) {
        glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &num);
    }
    return num;
}

const bool MR::MachineInfo::IsTextureStorageSupported() {
    static unsigned char state = 2;
    if(state == 2) {
        if(MR::MachineInfo::gl_version_major() >= 4 || GLEW_ARB_texture_storage) state = 1;
        else state = 0;
    }
    return (bool)state;
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

int MR::MachineInfo::MaxFragmentShaderTextureUnits() {
    static int s = 0;
    if(s == 0){
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &s);
        if(s == 0) {
            MR::Log::LogString("Failed MachineInfo::MaxFragmentShaderTextureUnits. MaxTextureUnits always 0. 8 will be used.", MR_LOG_LEVEL_ERROR);
            s = 8;
        }
    }
    return s;
}

int MR::MachineInfo::MaxVertexShaderTextureUnits() {
    static int s = 0;
    if(s == 0){
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &s);
        if(s == 0) {
            MR::Log::LogString("Failed MachineInfo::MaxVertexShaderTextureUnits. MaxTextureUnits always 0. 8 will be used.", MR_LOG_LEVEL_ERROR);
            s = 8;
        }
    }
    return s;
}

int MR::MachineInfo::MaxActivedTextureUnits() {
    static int s = 0;
    if(s == 0){
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &s);
        if(s == 0) {
            MR::Log::LogString("Failed MachineInfo::MaxActivedTextureUnits. MaxTextureUnits always 0. 8 will be used.", MR_LOG_LEVEL_ERROR);
            s = 8;
        }
    }
    return s;
}

std::string MR::MachineInfo::glsl_version_directive() {
    static std::string s = "";
    if(s == "") {
        switch(MR::MachineInfo::gl_version()) {
        case MR::MachineInfo::GLVersion::VUnknown:
        case MR::MachineInfo::GLVersion::VNotSupported:
        case MR::MachineInfo::GLVersion::Vx_x:
        case MR::MachineInfo::GLVersion::V3_2:
            s = std::string("#version 150 ") + (gl_core_profile() ? "core" : "");
            break;
        default:
            s = std::string("#version ")+std::to_string(MR::MachineInfo::gl_version_major())+std::to_string(MR::MachineInfo::gl_version_minor())+"0 " + (gl_core_profile() ? "core" : "");
            break;
        }
    }
    return s;
}

bool MR::MachineInfo::gl_core_profile() {
    return true;
}
