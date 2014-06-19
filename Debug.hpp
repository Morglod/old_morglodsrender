#pragma once

#ifndef _MR_DEBUG_H_
#define _MR_DEBUG_H_

#define _MR_ASSERTION_FUNC(x, y) std::cout << "Assertion failed (" << x << " == " << y << ") in \"" __FILE__  << "\" at " << __LINE__ << " line" << std::endl;

#define Assert(_what, _notThis) \
    if((_what) == _notThis) { \
        _MR_ASSERTION_FUNC( #_what , #_notThis ) \
    }

#define AssertAndExec(_what, _notThis, exec) \
    if((_what) == _notThis) { \
        _MR_ASSERTION_FUNC( #_what , #_notThis ) \
        exec; \
    }

#define AssertExec(_what, _notThis, exec) \
    if((_what) == _notThis) { \
        exec; \
    }


#endif // _MR_DEBUG_H_
