#pragma once

#include <Events.hpp>

#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace std {
    inline std::string to_string(const glm::vec2& v) {
        return std::to_string(v.x) + ", " + std::to_string(v.y);
    }

    inline std::string to_string(const glm::vec3& v) {
        return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z);
    }

    inline std::string to_string(const glm::vec4& v) {
        return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w);
    }
}

namespace mr {

class IObjectHandle {
public:
    mu::Event<IObjectHandle*> OnDestroy;
    virtual void Destroy() {}
    virtual ~IObjectHandle();
};

typedef std::shared_ptr<IObjectHandle> ObjectHandlePtr;

}
