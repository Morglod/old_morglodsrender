/** HEADER ONLY **/

#pragma once

#ifndef _MR_DEBUG_H_
#define _MR_DEBUG_H_

#ifndef _MR_ASSERTION_FUNC(x)
#define _MR_ASSERTION_FUNC(x) std::cout << "Assertion failed (" << x << ") in \"" __FILE__  << "\" at " << __LINE__ << " line" << std::endl;
#endif

#define Assert(_what ) \
    if(_what) { \
        _MR_ASSERTION_FUNC( #_what ) \
    }

#define AssertAndExec(_what, _exec) \
    if(_what) { \
        _MR_ASSERTION_FUNC( #_what ) \
        _exec; \
    }

#define AssertExec(_what, _exec) \
    if(_what) { \
        _exec; \
    }


#endif // _MR_DEBUG_H_
