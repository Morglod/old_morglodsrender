#pragma once

#include "Config.hpp"
#include <string>

namespace mr {
namespace machine {

    enum class GPUVendor : char {
        Null = -1,
        Other = 0,
        Nvidia = 1,
        ATI = 2,
        Intel = 3,
        Microsoft_DirectX_Wrapper = 4
    };

    const GPUVendor GetGpuVendor();
    const std::string GetGpuVendorAsString();

    const std::string GetGpuName();
    const std::string GetExtensionsListAsString();

    const unsigned int GetGpuTotalMemoryKb();
    const unsigned int GetGpuCurrentFreeMemoryKb(); //free mem

    void PrintInfo();
}

namespace gl {

    enum class GLVersion : unsigned char {
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

    const int GetMajorVersion();
    const int GetMinorVersion();
    const float GetVersionAsFloat();
    const bool IsOpenGL45(); //OpenGL 4.5 supported?

    const GLVersion GetVersion();
    const std::string GetVersionAsString();
    const std::string GetGlslVersionAsString();
    //const std::string GetGlslVersionDirective();

    const int GetMaxTextureSize();
    const int GetMaxFragmentShaderTextureUnits();
    const int GetMaxVertexShaderTextureUnits();
    const int GetMaxTextureUnits();
    const int GetMaxGeometryStreamsNum();
    const int GetMaxVertexAttribsNum();
    const int GetMaxFramebufferColorSlots();

    //const bool IsCoreProfile();

    const bool IsDirectStateAccessSupported();
    const bool IsVertexAttribBindingSupported();
    const bool IsTextureStorageSupported();
    const bool IsBufferStorageSupported();
    const bool IsNVVBUMSupported(); //direct access to gpu's addresses (for vertex buffers)
    const bool IsIndirectDrawSupported();
    const bool IndirectDraw_UseGPUBuffer();

    bool CheckError(std::string* errorOutput, int* glCode); //return true on error and print error's description in string; errorOutput and glCode can be nullptr
    void ClearError(); //set error status to no_error
}
}
