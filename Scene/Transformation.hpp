#pragma once

#ifndef _MR_TRANSFORMATION_H_
#define _MR_TRANSFORMATION_H_

#include "../Utils/Events.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

namespace MR{
class Transform{
public:
    MR::EventListener<Transform*, const glm::mat4&> OnChanged;

    void Calc();

    inline bool IsAutoRecalc(){ return _autoReCalc; }
    inline void SetAutoRecalc(const bool& state);

    void SetPos(const glm::vec3& p);
    void SetPos(glm::vec3* p);
    void SetRot(const glm::vec3& r);
    void SetRot(glm::vec3* r);
    void SetScale(const glm::vec3& s);
    void SetScale(glm::vec3* s);

    inline glm::vec3& GetPos(){ return _pos; }
    inline glm::vec3* GetPosP(){ return &_pos; }
    inline glm::vec3& GetRot(){ return _rot; }
    inline glm::vec3* GetRotP(){ return &_rot; }
    inline glm::vec3& GetScale(){ return _scale; }
    inline glm::vec3* GetScaleP(){ return &_scale; }
    inline glm::mat4& GetMat(){ return _mat; }
    inline glm::mat4* GetMatP(){ return &_mat; }

    Transform() : _mat(glm::mat4(1.0f)), _pos(glm::vec3(0.0f)), _rot(glm::vec3(0.0f)), _scale(glm::vec3(1.0f)), _autoReCalc(true) { Calc(); }
    Transform(glm::mat4 m) : _mat(m), _pos(glm::vec3(0.0f)), _rot(glm::vec3(0.0f)), _scale(glm::vec3(1.0f)), _autoReCalc(true) {}

    static glm::vec2 NormalizedDirection(float angle);
    static glm::vec3 NormalizedDirection(float angleX, float angleY, float angleZ);

    inline static glm::vec3 WorldForwardVector() { return glm::vec3(0,0,-1); }
    inline static glm::vec3 WorldBackwardVector() { return glm::vec3(0,0,1); }
    inline static glm::vec3 WorldUpVector() { return glm::vec3(0,1,0); }
    inline static glm::vec3 WorldDownVector() { return glm::vec3(0,-1,0); }
    inline static glm::vec3 WorldLeftVector() { return glm::vec3(-1,0,0); }
    inline static glm::vec3 WorldRightVector() { return glm::vec3(1,0,0); }
    inline static float CalcDist(const glm::vec3& v1, const glm::vec3& v2) { return glm::distance(v1, v2); }

protected:
    glm::mat4 _mat;
    glm::vec3 _pos;
    glm::vec3 _rot;
    glm::vec3 _scale;
    bool _autoReCalc;
};
}

#endif
