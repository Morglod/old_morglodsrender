#include "Viewport.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

mr::Viewport * usedViewport = nullptr;
mu::Event<mr::Viewport*> mr::Viewport::OnAnyUsed;

namespace mr {

void Viewport::SetRect(const glm::ivec4& r){
    if(_rect != r){
        _rect = r;

        if(usedViewport == this) {
            glViewport(_rect.x, _rect.y, _rect.z, _rect.w);
        }

        OnRectChanged(dynamic_cast<Viewport*>(this), r);
    }
}

bool Viewport::Use() {
    if(usedViewport != this) {
        usedViewport = this;
        glViewport(_rect.x, _rect.y, _rect.z, _rect.w);

        mr::Viewport::OnAnyUsed(dynamic_cast<Viewport*>(this));
        OnUsed(dynamic_cast<Viewport*>(this));
    }
    return true;
}

Viewport::Viewport(const glm::ivec4& r) :
    _rect(r) {
}

Viewport::~Viewport(){
}

}
