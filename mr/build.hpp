#pragma once

#ifndef MR_STATIC
    #ifdef BUILD_DLL
        #define MR_API __declspec(dllexport)
    #else
        #define MR_API __declspec(dllimport)
    #endif // BUILD_DLL
#endif // MR_STATIC

#define MR_MULTITHREAD 1
#define MR_MULTITHREAD_FUTURE MR_MULTITHREAD
#define MR_MULTITHREAD_SEMAPHORE MR_MULTITHREAD
