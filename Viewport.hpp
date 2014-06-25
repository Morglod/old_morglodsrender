#pragma once

#ifndef _MR_VIEWPORT_H_
#define _MR_VIEWPORT_H_

#include "Types.hpp"
#include "Utils/Events.hpp"

#include <glm/glm.hpp>

namespace MR {

class IViewport {
public:
    MR::EventListener<IViewport*, const glm::ivec4&> OnRectChanged;

    virtual glm::ivec4 GetRect() = 0;
    virtual void SetRect(const glm::ivec4& r) = 0;

    virtual ~IViewport() {}
};

class Viewport : public Object, public IViewport {
public:
    inline std::string ToString() override { return "Viewport("+std::to_string(_rect.x)+", "+std::to_string(_rect.y)+", "+std::to_string(_rect.z)+", "+std::to_string(_rect.w)+")"; }

    inline glm::ivec4 GetRect() override { return _rect; }
    void SetRect(const glm::ivec4& r) override;

    Viewport(const glm::ivec4& r);
    virtual ~Viewport();
protected:
    glm::ivec4 _rect; //x,y,w,h
};

}

#endif // _MR_VIEWPORT_H_
