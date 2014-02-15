#pragma once

#ifndef _MR_TRANSFORMATION_H_
#define _MR_TRANSFORMATION_H_

#include "pre.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace MR{
class Transform{
protected:
    glm::mat4 mat;
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;
    bool autoReCalc;

public:
    void Calc();

    inline bool IsAutoRecalc(){ return autoReCalc; }
    inline void SetAutoRecalc(const bool& state);

    void SetPos(const glm::vec3& p);
    void SetPos(glm::vec3* p);
    void SetRot(const glm::vec3& r);
    void SetRot(glm::vec3* r);
    void SetScale(const glm::vec3& s);
    void SetScale(glm::vec3* s);

    inline glm::vec3& GetPos(){ return pos; }
    inline glm::vec3* GetPosP(){ return &pos; }
    inline glm::vec3& GetRot(){ return rot; }
    inline glm::vec3* GetRotP(){ return &rot; }
    inline glm::vec3& GetScale(){ return scale; }
    inline glm::vec3* GetScaleP(){ return &scale; }
    inline glm::mat4& GetMat(){ return mat; }
    inline glm::mat4* GetMatP(){ return &mat; }

    Transform() : mat(glm::mat4(1.0f)), pos(glm::vec3(0.0f)), rot(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), autoReCalc(true) { Calc(); }
    Transform(glm::mat4 m) : mat(m), pos(glm::vec3(0.0f)), rot(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), autoReCalc(true) {}

    static glm::vec2 NormalizedDirection(float angle);
    static glm::vec3 NormalizedDirection(float angleX, float angleY, float angleZ);

    /*inline static void AngleFromDirection(const glm::vec3& dir, float* angleX, float* angleY, float* angleZ){
        return glm::vec3(
            //angleY = glm::angle( glm::acos(dir.y) );
            glm::cos( glm::radians(angleY) ) * glm::sin( glm::radians(angleX) ),
            glm::sin( glm::radians(angleY) ),
            glm::cos( glm::radians(angleY) ) * glm::cos( glm::radians(angleX) )
        );
    }*/

    inline static glm::vec3 WorldForwardVector() { return glm::vec3(0,0,-1); }
    inline static glm::vec3 WorldBackwardVector() { return glm::vec3(0,0,1); }
    inline static glm::vec3 WorldUpVector() { return glm::vec3(0,1,0); }
    inline static glm::vec3 WorldDownVector() { return glm::vec3(0,-1,0); }
    inline static glm::vec3 WorldLeftVector() { return glm::vec3(-1,0,0); }
    inline static glm::vec3 WorldRightVector() { return glm::vec3(1,0,0); }
    inline static float CalcDist(const glm::vec3& v1, const glm::vec3& v2) { return glm::distance(v1, v2); }
};
}

#endif
