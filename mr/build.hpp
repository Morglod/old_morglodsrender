#pragma once

#ifndef MR_STATIC
    #ifdef BUILD_DLL
        #define MR_API __declspec(dllexport)
    #else
        #define MR_API __declspec(dllimport)
    #endif // BUILD_DLL
#endif // MR_STATIC
