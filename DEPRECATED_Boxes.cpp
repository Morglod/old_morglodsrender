#include "Boxes.hpp"

namespace MR {

void Box::ReMake(glm::vec3* points, const size_t& num){
    if((points == nullptr) || num == 0) return;
    _min = _max = glm::vec3(points[0].x, points[0].y, points[0].z);
    for(size_t i = 0; i < num; ++i){
        _min.x = glm::min(_min.x, points[i].x);
        _min.y = glm::min(_min.y, points[i].y);
        _min.z = glm::min(_min.z, points[i].z);
        _max.x = glm::max(_max.x, points[i].x);
        _max.y = glm::max(_max.y, points[i].y);
        _max.z = glm::max(_max.z, points[i].z);
    }
    _size = _max - _min;
    _v = _size.x * _size.y * _size.z;
}

void Box::ReMake(const glm::vec3& min, const glm::vec3& max){
    _min = min; _max = max; _size = max - min;
    _v = _size.x * _size.y * _size.z;
}

Box::Box(const glm::vec3& min, const glm::vec3& max) : _min(min), _max(max), _size(max-min) {
    _v = _size.x * _size.y * _size.z;
}

Box::Box(glm::vec3* points, const size_t& num){
    ReMake(points, num);
    _size = _max - _min;
    _v = _size.x * _size.y * _size.z;
}

void BoundingBox::ReMake(glm::vec3* points, const size_t& num, const glm::mat4& model){
    for(size_t i = 0; i < num; ++i){
        _min.x = glm::min(_min.x, points[i].x);
        _min.y = glm::min(_min.y, points[i].y);
        _min.z = glm::min(_min.z, points[i].z);
        _max.x = glm::max(_max.x, points[i].x);
        _max.y = glm::max(_max.y, points[i].y);
        _max.z = glm::max(_max.z, points[i].z);
    }
    _size = _max - _min;
    _mat = model;
    RecalcAABB();
}

void BoundingBox::ReMake(const glm::vec3& min, const glm::vec3& max, const glm::mat4& model){
    _min = min; _max = max; _size = max - min;
    _mat = model;
    RecalcAABB();
}

bool BoundingBox::TestPoint(const glm::vec3& point) {
    if(point.x < _aabb.GetMin().x) return false;
    if(point.y < _aabb.GetMin().y) return false;
    if(point.z < _aabb.GetMin().z) return false;
    if(point.x > _aabb.GetMax().x) return false;
    if(point.y > _aabb.GetMax().y) return false;
    if(point.z > _aabb.GetMax().z) return false;
    return true;
}

bool BoundingBox::TestAABB(const glm::vec3& min, const glm::vec3& size) {
    if(TestPoint(min)) return true;
    if(TestPoint(min+size)) return true;
    if(TestPoint(min+size/2.0f)) return true;
    if(TestPoint(glm::vec3(min.x+size.x, min.y, min.z))) return true;
    if(TestPoint(glm::vec3(min.x, min.y+size.x, min.z))) return true;
    if(TestPoint(glm::vec3(min.x, min.y, min.z+size.z))) return true;
    if(TestPoint(glm::vec3(min.x+size.x, min.y+size.y, min.z))) return true;
    if(TestPoint(glm::vec3(min.x, min.y+size.x, min.z+size.z))) return true;
    if(TestPoint(glm::vec3(min.x+size.x, min.y, min.z+size.z))) return true;
    return false;
}

bool BoundingBox::TestSphere(const glm::vec3& point, const float& radius) {
    if(TestPoint(point)) return true;
    if(TestPoint(glm::vec3(point.x+radius, point.y, point.z))) return true;
    if(TestPoint(glm::vec3(point.x-radius, point.y, point.z))) return true;
    if(TestPoint(glm::vec3(point.x, point.y+radius, point.z))) return true;
    if(TestPoint(glm::vec3(point.x, point.y-radius, point.z))) return true;
    if(TestPoint(glm::vec3(point.x, point.y, point.z+radius))) return true;
    if(TestPoint(glm::vec3(point.x, point.y, point.z-radius))) return true;
    return false;
}

BoundingBox::BoundingBox(const glm::vec3& point, const float& radius, const glm::mat4& model) : _min(glm::vec3(point.x-radius, point.y-radius, point.z-radius)), _max(glm::vec3(point.x+radius, point.y+radius, point.z+radius)), _size(_max-_min), _mat(model), _aabb(0,1) {
    RecalcAABB();
}

BoundingBox::BoundingBox(glm::vec3* points, const size_t& num, const glm::mat4& model) : _aabb(0, 1) {
    for(size_t i = 0; i < num; ++i){
        _min.x = glm::min(_min.x, points[i].x);
        _min.y = glm::min(_min.y, points[i].y);
        _min.z = glm::min(_min.z, points[i].z);
        _max.x = glm::max(_max.x, points[i].x);
        _max.y = glm::max(_max.y, points[i].y);
        _max.z = glm::max(_max.z, points[i].z);
    }
    _size = _max - _min;
    _mat = model;
    RecalcAABB();
}

BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max, const glm::mat4& model) : _min(min), _max(max), _size(max-min), _mat(model), _aabb(min, max) {
    RecalcAABB();
}

}
