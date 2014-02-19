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
    inline const char* what() { return t; }
    Exception(const std::string& s) : t(s.c_str()) {}
protected:
    const char* t;
};

    void ThrowExceptions(const bool& state);
    bool ThrowExceptions();
}

#endif
