#ifndef _MR_MACHINE_INFO_H_
#define _MR_MACHINE_INFO_H_

#include "Config.hpp"

#include <string>

#define MINIMAL_GL_VERSION_MAJOR 3
#define MINIMAL_GL_VERSION_MINOR 2

namespace mr{
class IContext;

    namespace MachineInfo{

        /** GPU INFO **/

        enum class GPUVendor : unsigned char {
            Other = 0,
            Nvidia = 1,
            ATI = 2,
            Intel = 3,
            Microsoft_DirectX_Wrapper = 4
        };

        enum class GLVersion : unsigned char {
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
        float gl_versions_f();
        const bool gl_version_over_4_5();
        std::string gl_version_string();
        GLVersion gl_version();
        std::string gpu_vendor_string();
        GPUVendor gpu_vendor();
        std::string gpu_name();
        std::string gl_extensions_list();
        std::string gl_version_glsl();
        unsigned int total_memory_kb();
        unsigned int current_memory_kb(); //free mem

        void PrintInfo();

        /** EXTENSIONS **/
        const bool IsDirectStateAccessSupported();
        const bool IsVertexAttribBindingSupported();
        const bool IsTextureStorageSupported();
        const bool IsBufferStorageSupported();
        const bool IsIndirectDrawSupported();
        const bool IsNVVBUMSupported(); //direct access to gpu's addresses (for vertex buffers)

        /** EXT MISC **/
        const bool IndirectDraw_UseGPUBuffer();

        /** GL INFO **/
        bool CatchError(std::string* errorOutput, int* glCode); //return true on error and print error's description in string; errorOutput and glCode can be NULL
        void ClearError(); //set error status to no_error
        int MaxTextureSize();
        int MaxFragmentShaderTextureUnits();
        int MaxVertexShaderTextureUnits();
        int MaxActivedTextureUnits();
        int GetGeometryStreamsNum();

        std::string glsl_version_directive();
        bool gl_core_profile();
    }
}

#endif
