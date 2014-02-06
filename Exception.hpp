#pragma once

#ifndef _MR_EXCEPTION_H_
#define _MR_EXCEPTION_H_

namespace MR {
class Exception : public std::exception {
public:
    const char* what() { return t; }
    Exception(const std::string& s) : t(s.c_str()) {}
protected:
    const char* t;
};
}

#endif
