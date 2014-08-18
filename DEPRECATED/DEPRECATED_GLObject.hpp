#pragma once

#ifndef _MR_GL_OBJECT_H_
#define _MR_GL_OBJECT_H_

#include <string>

namespace MR {
namespace GL {

class IContext;

class IGLObject {
public:
    enum class ObjectType : unsigned int {
        Buffer = 0x82E0,
        Shader = 0x82E1,
        Program = 0x82E2,
        Query = 0x82E3,
        ProgramPipeline = 0x82E4,
        Sampler = 0x82E6,
        DisplayList = 0x82E7,

        OBJECT_TYPES_NUM
    };

    virtual void Release() = 0;
    virtual bool IsAvailable() = 0;

    IGLObject(IContext* ctx, const ObjectType& type, unsigned int * handle);
    IGLObject(IContext* ctx, const ObjectType& type, unsigned int * handle, const std::string& name);
    virtual ~IGLObject();

    virtual inline IContext* GetContext() { return _context; }

    inline std::string GetName() { return _name; }
    virtual void SetName(const std::string& name);

private:
    IContext* _context;
    std::string _name;
    ObjectType _type;
    unsigned int * _handle_ptr;
};

}
}

#endif // _MR_GL_OBJECT_H
