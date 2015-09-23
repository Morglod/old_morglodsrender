#pragma once

#include "../build.hpp"

#include <inttypes.h>
#include <memory>

namespace mr {

typedef std::shared_ptr<class ObjectGL> ObjectGLPtr;

class MR_API ObjectGL final {
public:
    enum Type : uint8_t {
        T_Unknown = 0,
        T_Buffer = 1,

        T_Num
    };

    void Destroy();
    inline uint32_t GetId() const;
    inline ObjectGL::Type GetType() const;

    ObjectGL() = default;
    ObjectGL(uint32_t id, ObjectGL::Type type);
    ~ObjectGL();

private:
    uint32_t _id = 0;
    Type _type = ObjectGL::Type::T_Unknown;
};

inline uint32_t ObjectGL::GetId() const {
    return _id;
}

inline ObjectGL::Type ObjectGL::GetType() const {
    return _type;
}

}
