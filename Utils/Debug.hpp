/** HEADER ONLY **/

#pragma once

#ifndef _MR_DEBUG_H_
#define _MR_DEBUG_H_

/* DEFINE TOGGLE */
#undef MR_CHECK_DEFINED_TO_ONE_RESULT

//#define MR_CHECK_DEFINED_TO_ONE x

#if MR_CHECK_DEFINED_TO_ONE == 1
    #define MR_CHECK_DEFINED_TO_ONE_RESULT
#endif

/* -- */

//#define MR_DEBUG_USE_STD_IO //use std::cout
#define MR_DEBUG_USE_MR_LOG //use MR::Log::LogString

#ifndef _MR_ASSERTION_FUNC
#ifdef MR_DEBUG_USE_STD_IO
#include <iostream>
#define _MR_ASSERTION_FUNC(x) std::cout << x " in \"" __FILE__  << "\" at " << __LINE__ << " line" << std::endl;
#endif
#ifdef MR_DEBUG_USE_MR_LOG
#include "Log.hpp"
#define _MR_ASSERTION_FUNC(x) mr::Log::LogString(x + std::string(" in \"") + std::string(__FILE__) + "\" at " + std::to_string(__LINE__) + " line");
#endif
#endif

#define _MR_ASSERTION_MSG(x) _MR_ASSERTION_FUNC("Assertion failed (" x ")")

#define Assert(_what ) \
    if(!(_what)) { \
        _MR_ASSERTION_MSG( #_what ) \
    }

#define AssertAndExec(_what, _exec) \
    if(!(_what)) { \
        _MR_ASSERTION_MSG( #_what ) \
        _exec; \
    }

#define AssertExec(_what, _exec) \
    if(!(_what)) { \
        _exec; \
    }

#define MR_DBG_MSG \
    _MR_ASSERTION_FUNC("DEBUG MSG")

/** OPENGL DEBUG **/

#ifndef _MR_DEBUG_GL_ERROR_CATCHED_FUNC
    #define _MR_DEBUG_GL_ERROR_CATCHED_FUNC(std_string, int_gl_code) _MR_ASSERTION_FUNC("OpenGL error " + std::to_string(int_gl_code) + ": " + std_string)
#endif

#define CheckGLError(_ofLine, _errCode) \
    mr::gl::ClearError(); \
    _ofLine \
    { \
        std::string localErrorString = ""; \
        int localGlCode = 0; \
        if(mr::gl::CheckError(&localErrorString, &localGlCode)) { \
            _MR_DEBUG_GL_ERROR_CATCHED_FUNC(localErrorString, localGlCode) \
            _errCode \
        } \
    }

#endif // _MR_DEBUG_H_
