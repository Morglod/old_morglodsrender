#include "Context.hpp"
#include "GLObject.hpp"

#include <algorithm>

MR::GL::IContext* currentContext = nullptr;

namespace MR{
namespace GL {

IContext::IContext() {
}

IContext::~IContext() {
}

void SetCurrent(IContext* ctx){
    currentContext = ctx;
}

IContext* GetCurrent(){
    return currentContext;
}

}
}
