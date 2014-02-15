#ifndef _MR_MACHINE_INFO_H_
#define _MR_MACHINE_INFO_H_

#include "pre.hpp"

#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

#define MINIMAL_GL_VERSION_MAJOR 3
#define MINIMAL_GL_VERSION_MINOR 2

namespace MR{
    namespace MachineInfo{
        enum GPUVendor : unsigned char {
            Other = 0,
            Nvidia = 1,
            ATI = 2,
            Intel = 3,
            Microsoft_DirectX_Wrapper = 4
        };

        enum GLVersion : unsigned char {
            VUnknown = 255,
            VNotSupported = 0,
            Vx_x = 1,
            V3_2 = 2,
            V3_3 = 3,
            V3_x = 4,
            V4_0 = 5,
            V4_2 = 6,
            V4_3 = 7,
            V4_4 = 8,
            V4_x = 9
        };

        int gl_version_major();
        int gl_version_minor();
        std::string gl_version_string();
        GLVersion gl_version();
        std::string gpu_vendor_string();
        GPUVendor gpu_vendor();
        std::string gpu_name();
        std::string gl_extensions_list();
        std::string gl_version_glsl();
        int total_memory_kb();
        int current_memory_kb();
    }
}

#endif