#include "GLObject.hpp"
#include "Context.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

void MR::GL::IGLObject::SetName(const std::string& name) {
    _name = name;
    //if(_handle_ptr != 0) glObjectLabel((unsigned int)_type, *_handle_ptr, _name.size(), &_name[0]);
}

MR::GL::IGLObject::IGLObject(IContext* ctx, const ObjectType& type, unsigned int* handle) : _context(ctx), _name("auto"), _type(type), _handle_ptr(handle) {
    //_context->RegisterObject(this);
}

MR::GL::IGLObject::IGLObject(IContext* ctx, const ObjectType& type, unsigned int* handle, const std::string& name) : _context(ctx), _name(name), _type(type), _handle_ptr(handle) {
    //_context->RegisterObject(this);

    SetName(name);
}

MR::GL::IGLObject::~IGLObject() {
    //_context->UnRegisterObject(this);
}
