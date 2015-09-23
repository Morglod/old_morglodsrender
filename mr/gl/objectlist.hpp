#pragma once

#include "object.hpp"

#include <vector>

namespace mr {

class ObjectGL;

class MR_API ObjectListGL final {
public:
    enum Type : uint8_t {
        T_Handle = 0,
        T_Destroy = 1,

        T_Num
    };

    void Insert(ObjectGL* o);
    void Exec();

    ObjectListGL() = default;
    ~ObjectListGL() = default;
    ObjectListGL(ObjectListGL::Type const& cmd_type, ObjectGL::Type const& obj_type);

    static ObjectListGL* GetBufferDestroyList();

private:
    std::vector<uint32_t> _ids;
    ObjectListGL::Type _cmd_type;
    ObjectGL::Type _obj_type;

    typedef void (*_ExecFunc)(uint32_t num, uint32_t* ids);
};

}
