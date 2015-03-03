#pragma once

#include "Events.hpp"
#include <glm/glm.hpp>

namespace mr {

class Viewport {
public:
    static mu::Event<Viewport*> OnAnyUsed;

    mu::Event<Viewport*, glm::ivec4 const&> OnRectChanged;
    mu::Event<Viewport*> OnUsed;

    bool Use();

    inline glm::ivec4 GetRect() { return _rect; }
    void SetRect(glm::ivec4 const& r);

    Viewport(glm::ivec4 const& r);
    virtual ~Viewport();
protected:
    glm::ivec4 _rect; //x,y,w,h
};

}
