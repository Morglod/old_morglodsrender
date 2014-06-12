#pragma once

#ifndef _MR_BOXES_H_
#define _MR_BOXES_H_

#include <glm/glm.hpp>
#include "Types.hpp"

namespace MR {

class IBox {
public:
    virtual glm::vec3 GetMin() = 0;
    virtual glm::vec3 GetMax() = 0;
    virtual glm::vec3 GetSize() = 0; // min + size = max
    virtual float GetVolume() = 0;
};

class Box : public IBox {
public:
    inline glm::vec3 GetMin() override { return _min; }
    inline glm::vec3 GetMax() override { return _max; }
    inline glm::vec3 GetSize() override { return _size; }
    inline float GetVolume() override { return _v; }

    virtual void ReMake(glm::vec3* points, const size_t& num);
    virtual void ReMake(const glm::vec3& min, const glm::vec3& max);

    //Make box from it's min and max points
    Box(const glm::vec3& min, const glm::vec3& max);

    //Find min and max points and make a box
    Box(glm::vec3* points, const size_t& num);
protected:
    glm::vec3 _min, _max, _size;
    float _v;
};

class BoundingBox : public IBox, public ICollidable {
public:
    inline glm::vec3 GetMin() override { return glm::vec3(_mat * glm::vec4(_min, 1.0f)); }
    inline glm::vec3 GetMax() override { return glm::vec3(_mat * glm::vec4(_max, 1.0f)); }
    inline glm::vec3 GetSize() override { return GetMax() - GetMin(); }
    inline float GetVolume() override {
        glm::vec3 sz = GetSize();
        return sz.x * sz.y * sz.z;
    }

    inline glm::mat4 GetModelMat() { return _mat; }
    inline void SetModelMat(const glm::mat4& mat) { _mat = mat; RecalcAABB(); }

    bool TestPoint(const glm::vec3& point) override;
    bool TestAABB(const glm::vec3& min, const glm::vec3& size) override;
    bool TestSphere(const glm::vec3& point, const float& radius) override;

    inline void RecalcAABB() {
        glm::vec3 p[4]{
            glm::vec3(_mat * glm::vec4(_min, 1.0f)),
            glm::vec3(_mat * glm::vec4(_min.x+_size.x, _min.y, _min.z, 1.0f)),
            glm::vec3(_mat * glm::vec4(_min.x, _min.y+_size.y, _min.z, 1.0f)),
            glm::vec3(_mat * glm::vec4(_min.x, _min.y, _min.z+_size.z, 1.0f))
        };
        _aabb.ReMake(&p[0], 4);
    }

    virtual void ReMake(glm::vec3* points, const size_t& num, const glm::mat4& model);
    virtual void ReMake(const glm::vec3& min, const glm::vec3& max, const glm::mat4& model);

    BoundingBox(const glm::vec3& point, const float& radius, const glm::mat4& model);
    BoundingBox(glm::vec3* points, const size_t& num, const glm::mat4& model);
    BoundingBox(const glm::vec3& min, const glm::vec3& max, const glm::mat4& model);
protected:
    glm::vec3 _min, _max, _size;
    glm::mat4 _mat;
    Box _aabb;
};

}

#endif // _MR_BOXES_H_
