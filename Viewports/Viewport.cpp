#include "Viewport.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

MR::Viewport * usedViewport = nullptr;

namespace MR {

void Viewport::SetRect(const glm::ivec4& r){
    if(_rect != r){
        _rect = r;

        if(usedViewport == this) {
            glViewport(_rect.x, _rect.y, _rect.z, _rect.w);
        }

        OnRectChanged(dynamic_cast<IViewport*>(this), r);
    }
}

bool Viewport::Use() {
    if(usedViewport != this) {
        usedViewport = this;
        glViewport(_rect.x, _rect.y, _rect.z, _rect.w);

        MR::Viewport::OnAnyUsed(dynamic_cast<IViewport*>(this));
        OnUsed(dynamic_cast<IViewport*>(this));
    }
    return true;
}

Viewport::Viewport(const glm::ivec4& r) :
    _rect(r) {
}

Viewport::~Viewport(){
}

}
