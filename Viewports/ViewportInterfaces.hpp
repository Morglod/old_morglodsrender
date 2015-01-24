#pragma once

#ifndef _MR_VIEWPORT_INTERFACES_H_
#define _MR_VIEWPORT_INTERFACES_H_

#include "../Types.hpp"

#include <glm/glm.hpp>

namespace mr {

class IViewport {
public:
    static mu::Event<IViewport*> OnAnyUsed;

    mu::Event<IViewport*, const glm::ivec4&> OnRectChanged;
    mu::Event<IViewport*> OnUsed;

    virtual glm::ivec4 GetRect() = 0;
    virtual void SetRect(const glm::ivec4& r) = 0;
    virtual bool Use() = 0;

    virtual ~IViewport() {}
};

}

#endif // _MR_VIEWPORT_H_
