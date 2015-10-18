#include "mr/gl/object.hpp"
#include "mr/gl/objectlist.hpp"

/**

TEST ONLY

**/

namespace mr {

void ObjectGL::Destroy() {
    ObjectListGL::GetBufferDestroyList()->Insert(this);
    _id = 0;
}

ObjectGL::ObjectGL(uint32_t id, ObjectGL::Type type) : _id(id), _type(type) {
}

ObjectGL::~ObjectGL() {
    Destroy();
}

}
