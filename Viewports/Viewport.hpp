#pragma once

#ifndef _MR_VIEWPORT_H_
#define _MR_VIEWPORT_H_

#include "ViewportInterfaces.hpp"

namespace mr {

class Viewport : public IViewport {
public:
    bool Use() override;

    inline glm::ivec4 GetRect() { return _rect; }
    void SetRect(const glm::ivec4& r);

    Viewport(const glm::ivec4& r);
    virtual ~Viewport();
protected:
    glm::ivec4 _rect; //x,y,w,h
};

}

#endif // _MR_VIEWPORT_H_
