#include "Debug.hpp"
#include "Context.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace MR {
namespace GL {

void Debug::DebugOutput(const bool& state) {
    _debug_state = state;
    if(state) glEnable(GL_DEBUG_OUTPUT);
    else glDisable(GL_DEBUG_OUTPUT);
}

Debug::Debug(IContext* ctx) : _debug_state(false), _context_ptr(ctx) {
}

Debug::~Debug() {
    _context_ptr->MakeCurrent();
    if(_debug_state) glDisable(GL_DEBUG_OUTPUT);
}

}
}
