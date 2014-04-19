#include "Viewport.hpp"

namespace MR {

void Viewport::SetRect(const glm::ivec4& r){
    if(_rect != r){
        _rect = r;
        OnRectChanged(this, r);
    }
}

Viewport::Viewport(const glm::ivec4& r) :
    _rect(r) {
}

Viewport::~Viewport(){
}

}
