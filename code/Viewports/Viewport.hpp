#pragma once

#include <glm/glm.hpp>

namespace mr {

class Viewport {
public:
    bool Use();

    inline glm::ivec4 GetRect() {
        return _rect;
    }
    void SetRect(glm::ivec4 const& r);

    Viewport(glm::ivec4 const& r);
    virtual ~Viewport();
protected:
    glm::ivec4 _rect; //x,y,w,h
};

}
