#include "Viewport.hpp"

#include <GL/glew.h>

namespace mr {

void Viewport::SetRect(const glm::ivec4& r){
    if(_rect != r){
        _rect = r;
    }
}

bool Viewport::Use() {
    glViewport(_rect.x, _rect.y, _rect.z, _rect.w);
    return true;
}

Viewport::Viewport(const glm::ivec4& r) :
    _rect(r) {
}

Viewport::~Viewport(){
}

}
