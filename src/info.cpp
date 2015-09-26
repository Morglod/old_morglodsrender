#include "mr/info.hpp"
#include "mr/build_info.hpp"
#include "mr/log.hpp"
#include "mr/pre/glew.hpp"

#include <unordered_map>

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

namespace mr {

const std::unordered_map<std::string, Info::GPUVendor> __MR__StringToGpuVendor({
    {"NVIDIA Corporation", Info::GPUVendor::Nvidia},
    {"ATI Technologies", Info::GPUVendor::ATI},
    {"ATI Technologies Inc.", Info::GPUVendor::ATI},
    {"INTEL", Info::GPUVendor::Intel},
    {"Microsoft", Info::GPUVendor::Microsoft_DirectX_Wrapper}
});

Info::GPUVendor Info::GetGpuVendor() {
    static GPUVendor vendor = GPUVendor::Null;
    if(vendor == GPUVendor::Null)
        vendor = (__MR__StringToGpuVendor.count(GetGpuVendorAsString()) != 0) ? (GPUVendor)__MR__StringToGpuVendor.at(GetGpuVendorAsString()) : GPUVendor::Other;
    return vendor;
}

const std::string Info::GetGpuVendorAsString() {
    static std::string outv = (const char*)glGetString(GL_VENDOR);
    return outv;
}

const std::string Info::GetGpuName() {
    static std::string outv = (const char*)glGetString(GL_RENDERER);
    return outv;
}

const std::string Info::GetExtensionsListAsString() {
    static std::string outv = (const char*)glGetString(GL_EXTENSIONS);
    return outv;
}

void Info::PrintInfo() {
    try {
    MR_LOG(
            std::string("\nVersion: ") + GetVersionAsString() +
            std::string("\nGLSL: ") + GetGlslVersionAsString() +
            std::string("\nOpenGL: ") + std::to_string(GetMajorVersion()) + std::string(" ") + std::to_string(GetMinorVersion()) +
            std::string("\nGPU: ") + GetGpuName() + std::string(" from ") + GetGpuVendorAsString() +
            std::string("\nMem Total(kb): ") + std::to_string(GetGpuTotalMemoryKb()) + std::string(" Current free (kb): ") + std::to_string(GetGpuCurrentFreeMemoryKb()) + "\n\n"
        );

        MR_LOG("\nNvidia VBUM: " + std::to_string(IsNVVBUMSupported()));
        MR_LOG("Direct state access EXT: " + std::to_string(IsDSA_EXT()));
        MR_LOG("Direct state access ARB: " + std::to_string(IsDSA_ARB()));
    } catch(std::exception& e) {
        MR_LOG_ERROR(Info::PrintInfo, "Exception catched, while printing machine info.");
    }
    ClearError();
}

uint32_t Info::GetGpuTotalMemoryKb() {
    static uint32_t total_mem_kb = 0;
    if(total_mem_kb != 0) return total_mem_kb;
    if(GetGpuVendor() == GPUVendor::Nvidia) {
        glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, (int*)&total_mem_kb);
    }
    else {
        MR_LOG_WARNING(Info::GetGpuTotalMemoryKb, "Get total memory on other than Nvidia vendors is not yet supported. 8gb will be used.");
        total_mem_kb = 8388608;
    }
    /*else if(gpu_vendor() == GPUVendor::ATI) {
        //glGetIntegerv(VBO_FREE_MEMORY_ATI, (int*)&total_mem_kb);
        if(wglGetGPUIDsAMD == 0) {
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
        //total_mem_kb /= 1024;
    }*/
    return total_mem_kb;
}

uint32_t Info::GetGpuCurrentFreeMemoryKb() {
    static bool firstCall = true;
    static uint32_t cur_avail_mem_kb = 0;
    if(firstCall) {
        firstCall = false;
        if(GetGpuVendor() == GPUVendor::Nvidia) glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, (int*)&cur_avail_mem_kb);
        else if(GetGpuVendor() == GPUVendor::ATI){
            glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, (int*)&cur_avail_mem_kb);
        } else {
            MR_LOG_WARNING(Info::GetGpuCurrentFreeMemoryKb, "Get current free memory on other than Nvidia & ATI vendors is not yet supported. 8gb will be used.");
            cur_avail_mem_kb = 8388608;
        }
    }
    return cur_avail_mem_kb;
}

int32_t Info::GetMajorVersion() {
    static int32_t outv = 0;
    if(outv == 0) glGetIntegerv(GL_MAJOR_VERSION, &outv);
    return outv;
}

int32_t Info::GetMinorVersion() {
    static int32_t outv = 0;
    if(outv == 0) glGetIntegerv(GL_MINOR_VERSION, &outv);
    return outv;
}

float Info::GetVersionAsFloat() {
    static float f = 0.0f;
    if(f < 0.1f && f > -0.1f) {
        float m = (float)GetMinorVersion();
        while((int32_t)m != 0) {
            m /= 10.0f;
        }
        f = (float)GetMajorVersion() + m;
    }
    return f;
}

bool Info::IsOpenGL45() {
    static int8_t b = -1;
    if(b == -1) {
        if(GetMajorVersion() == 4) {
            if(GetMinorVersion() >= 5) b = 1;
            else b = 0;
        }
        else if(GetMajorVersion() > 4) b = 1;
        else b = 0;
    }
    return (bool)b;
}

const std::string Info::GetVersionAsString() {
    static std::string outv = (const char*)glGetString(GL_VERSION);
    return outv;
}

Info::GLVersion Info::GetVersion() {
    static int32_t gl_major = GetMajorVersion();
    static int32_t gl_minor = GetMinorVersion();
    static GLVersion ver = GLVersion::VUnknown;

    if(ver != GLVersion::VUnknown) return ver;

    if( (gl_major < MR_MINIMAL_GL_VERSION_MAJOR) && (gl_minor < MR_MINIMAL_GL_VERSION_MINOR) ) {
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

    return GLVersion::VNotSupported;
}

const std::string Info::GetGlslVersionAsString() {
    static std::string outv = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    return outv;
}

/*const std::string GetGlslVersionDirective() {
    static std::string s = "";
    if(s == "") {
        switch(mr::MachineInfo::gl_version()) {
        case mr::MachineInfo::GLVersion::VUnknown:
        case mr::MachineInfo::GLVersion::VNotSupported:
        case mr::MachineInfo::GLVersion::Vx_x:
        case mr::MachineInfo::GLVersion::V3_2:
            s = std::string("#version 150 ") + (gl_core_profile() ? "core" : "");
            break;
        default:
            s = std::string("#version ")+std::to_string(mr::MachineInfo::gl_version_major())+std::to_string(mr::MachineInfo::gl_version_minor())+"0 " + (gl_core_profile() ? "core" : "");
            break;
        }
    }
    return s;
}*/

bool Info::IsNVVBUMSupported(){
    static bool support = GLEW_NV_vertex_buffer_unified_memory;
    return support;
}

bool Info::IsIndirectDrawSupported() {
    static bool support = GLEW_ARB_draw_indirect;//(__glewDrawArraysIndirect);
    return support;
}

bool Info::IsDSA_EXT() {
    static bool support = GLEW_EXT_direct_state_access;
    return support;
}

bool Info::IsDSA_ARB() {
    static bool support = GLEW_ARB_direct_state_access;
    return support;
}

bool Info::IndirectDraw_UseGPUBuffer() {
    return IsIndirectDrawSupported();
}

bool Info::IsVertexAttribBindingSupported() {
    static bool state = GLEW_ARB_vertex_attrib_binding;
    return state;
}

bool Info::IsBufferStorageSupported() {
    static bool state = (__glewBufferStorage);
    return state;
}

int32_t Info::GetMaxGeometryStreamsNum() {
    static int32_t num = -10;
    if(num == -10) {
        glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &num);
    }
    return num;
}

bool Info::IsTextureStorageSupported() {
    static uint8_t state = 2;
    if(state == 2) {
        if(GetMajorVersion() >= 4 || GLEW_ARB_texture_storage) state = 1;
        else state = 0;
    }
    return (bool)state;
}

bool Info::IsBindlessTextureSupported(bool& out_ARB) {
    static bool stateARB = GLEW_ARB_bindless_texture;
    static bool state = GLEW_ARB_bindless_texture || GLEW_NV_bindless_texture;
    out_ARB = stateARB;
    return state;
}

bool Info::CheckError(std::string* errorOutput, int32_t * glCode){
    GLenum er = glGetError();
    if(glCode) *glCode = er;

    if(errorOutput) {
        switch(er){
        default:
        case GL_NO_ERROR:
            *errorOutput = "No error";
            break;
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
            MR_LOG_ERROR(Info::CheckError, "Out of memory");
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

void Info::ClearError(){
    glGetError();
}

int32_t Info::GetMaxTextureSize(){
    static int32_t s = -10;
    if(s == -10){
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &s);
    }
    return s;
}

int32_t Info::GetMaxFragmentShaderTextureUnits() {
    static int32_t s = -1;
    if(s == -1){
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &s);
        if(s == 0) {
            MR_LOG_ERROR(Info::GetMaxFragmentShaderTextureUnits, "Max texture units is 0. 8 will be used.");
            s = 8;
        }
    }
    return s;
}

int32_t Info::GetMaxVertexShaderTextureUnits() {
    static int32_t s = -1;
    if(s == -1){
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &s);
        if(s == 0) {
            MR_LOG_ERROR(Info::GetMaxVertexShaderTextureUnits, "Max vertex texture units is 0. 8 will be used.");
            s = 8;
        }
    }
    return s;
}

int32_t Info::GetMaxTextureUnits() {
    static int32_t s = -1;
    if(s == -1){
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &s);
        if(s == 0) {
            MR_LOG_ERROR(Info::GetMaxTextureUnits, "Max combined texture units is 0. 8 will be used.");
            s = 8;
        }
    }
    return s;
}

int32_t Info::GetMaxVertexAttribsNum() {
    static int32_t s = -1;
    if(s == -1){
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &s);
        if(s == 0) {
            MR_LOG_ERROR(Info::GetMaxVertexAttribsNum, "Max vertex attribs is 0. 16 will be used.");
            s = 16;
        }
    }
    return s;
}

int32_t Info::GetMaxFramebufferColorSlots() {
    static int32_t s = -1;
    if(s == -1){
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &s);
        if(s == 0) {
            MR_LOG_ERROR(Info::GetMaxFramebufferColorSlots, "Max framebuffer color attachments is 0. 15 will be used.");
            s = 15;
        }
    }
    return s;
}

}
