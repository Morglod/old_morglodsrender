#include "Context.hpp"

MR::GL::IContext* currentContext = nullptr;

namespace MR{
namespace GL {

void IContext::Free(){
}

void SetCurrent(IContext* ctx){
    currentContext = ctx;
}

IContext* GetCurrent(){
    return currentContext;
}

}
}
