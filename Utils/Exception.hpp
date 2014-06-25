/**
    EXCEPTIONS IN MR

    [What Problem][Where]. [Description or possible reason]

    For example:

    "Failed LoadGLTexture. File not found" or
    "Failed Texture::CreateOpenGLTexture. blablabla"
*/

#pragma once

#ifndef _MR_EXCEPTION_H_
#define _MR_EXCEPTION_H_

#include <string>

namespace MR {

class Exception : public std::exception {
public:
    const char* what() { return t.c_str(); }
    Exception(const std::string& s) : t(s) {}
protected:
    std::string t;
};

    void ThrowExceptions(const bool& state);
    void ThrowCriticalExceptions(const bool& state);

    bool ThrowExceptions();
    bool ThrowCriticalExceptions();
}

#endif
