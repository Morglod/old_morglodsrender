#include "mr/gl/objectlist.hpp"
#include "mr/pre/glew.hpp"
#include "mr/log.hpp"

/**

TEST ONLY

**/

namespace {

void _DestroyUnknown(uint32_t num, uint32_t* ids) {
    ((void)num); ((void)ids);
    MR_LOG_WARNING(ObjectListGL::Exec, "Can't destroy unknown objects");
}

void _DestroyBuffers(uint32_t num, uint32_t* ids) {
    glDeleteBuffers(num, ids);
}

mr::ObjectListGL _buffer_destroy_list(mr::ObjectListGL::Type::T_Destroy, mr::ObjectGL::Type::T_Buffer);

}

namespace mr {

ObjectListGL* ObjectListGL::GetBufferDestroyList() {
    return &_buffer_destroy_list;
}

void ObjectListGL::Insert(ObjectGL* o) {
    if(o->GetType() != _obj_type) {
        MR_LOG_ERROR(ObjectListGL::Insert, "Can't insert gl object of different type");
    }
    _ids.push_back(o->GetId());
}

void ObjectListGL::Exec() {
    if(_ids.empty()) return;

    const _ExecFunc destroy_func[ObjectGL::Type::T_Num] = {
        _DestroyUnknown,
        _DestroyBuffers
    };

    switch(_cmd_type) {
    case ObjectListGL::Type::T_Destroy:{
        destroy_func[_obj_type](_ids.size(), _ids.data());
        }break;

    case ObjectListGL::Type::T_Handle:{}break;
    case ObjectListGL::Type::T_Num:
    default:{
        MR_LOG_WARNING(ObjectListGL::Exec, "Unknown cmd type");
        }break;
    }
}

ObjectListGL::ObjectListGL(ObjectListGL::Type const& cmd_type, ObjectGL::Type const& obj_type) : _ids(), _cmd_type(cmd_type), _obj_type(obj_type) {
}

}
