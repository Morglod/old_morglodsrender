#pragma once

#include "Build.hpp"

#include <inttypes.h>
#include <string>

namespace mr {

class MR_API Info final {
public:
    enum class GPUVendor : int8_t {
        Null = -1,
        Other = 0,
        Nvidia = 1,
        ATI = 2,
        Intel = 3,
        Microsoft_DirectX_Wrapper = 4
    };

    static GPUVendor GetGpuVendor();
    static const std::string GetGpuVendorAsString();

    static const std::string GetGpuName();
    static const std::string GetExtensionsListAsString();

    static uint32_t GetGpuTotalMemoryKb();
    static uint32_t GetGpuCurrentFreeMemoryKb(); //free mem

    static void Print();

    enum class GLVersion : uint8_t {
        VUnknown = 255,
        VNotSupported = 0,
        V3_2 = 1,
        V3_3,
        V3_x,
        V4_0,
        V4_2,
        V4_3,
        V4_4,
        V4_5,
        V4_x
    };

    static int32_t GetMajorVersion();
    static int32_t GetMinorVersion();
    static float GetVersionAsFloat();
    static bool IsOpenGL45(); //OpenGL 4.5 supported?

    static GLVersion GetVersion();
    static const std::string GetVersionAsString();
    static const std::string GetGlslVersionAsString();
    //const std::string GetGlslVersionDirective();

    static int32_t GetMaxTextureSize();
    static int32_t GetMaxFragmentShaderTextureUnits();
    static int32_t GetMaxVertexShaderTextureUnits();
    static int32_t GetMaxTextureUnits();
    static int32_t GetMaxGeometryStreamsNum();
    static int32_t GetMaxVertexAttribsNum();
    static int32_t GetMaxFramebufferColorSlots();

    //const bool IsCoreProfile();

    static bool IsDSA_EXT(); //DirectStateAccess Extension
    static bool IsDSA_ARB(); //DirectStateAccess from OpenGL 4.5

    static bool IsVertexAttribBindingSupported();
    static bool IsTextureStorageSupported();
    static bool IsBufferStorageSupported();
    static bool IsNVVBUMSupported(); //direct access to gpu's addresses (for vertex buffers)
    static bool IsIndirectDrawSupported();
    static bool IndirectDraw_UseGPUBuffer();
    static bool IsBindlessTextureSupported(bool& out_ARB);

    static bool CheckError(std::string* errorOutput, int32_t* glCode); //return true on error and print error's description in string; errorOutput and glCode can be nullptr
    static void ClearError(); //set error status to no_error
};

}
