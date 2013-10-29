#ifndef _MR_MACHINE_INFO_H_
#define _MR_MACHINE_INFO_H_

#include <string>

#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

namespace MR{
    namespace MachineInfo{
        enum GPUVendor{
            Other = 0,
            Nvidia = 1,
            ATI = 2,
            Intel = 3,
            Microsoft_DirectX_Wrapper = 4
        };

        int gl_version_major();
        int gl_version_minor();
        std::string version_string();
        std::string gpu_vendor_string();
        GPUVendor gpu_vendor();
        std::string gpu_name();
        std::string extensions_list();
        std::string version_glsl();
        int total_memory_kb();
        int current_memory_kb();
    }
}

#endif
