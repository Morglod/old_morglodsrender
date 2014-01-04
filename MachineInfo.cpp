#include "MachineInfo.hpp"

#include <windows.h>

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
    else if(v == "ATI Technologies") return GPUVendor::Nvidia;
    else if(v == "INTEL") return GPUVendor::Nvidia;
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
    if(gpu_vendor() == Nvidia) glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
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
            return 0;
        }

        int s = ExtEscape(hdc, 0x7032, 0x14, (LPCSTR)i, 0x14, (LPSTR)o);

        DeleteDC(hdc);

        if (s <= 0) {
            return 0;
        }

        return (o[3] * 1048576)*1024;
    }
    return total_mem_kb;
}
int MR::MachineInfo::current_memory_kb() {
    GLint cur_avail_mem_kb = 0;
    if(gpu_vendor() == Nvidia) glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);
    return cur_avail_mem_kb;
}
