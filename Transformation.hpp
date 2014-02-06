#pragma once

#ifndef _MR_TRANSFORMATION_H_
#define _MR_TRANSFORMATION_H_

#include "pre.hpp"

namespace MR{
class Transform{
protected:
    glm::mat4 mat;
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;
    bool autoReCalc;

public:
    inline void Calc(){
        /*glm::mat4 tm = glm::translate(glm::mat4(1.0f), pos);
        glm::mat4 rmx = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1,0,0));
        glm::mat4 rmy = glm::rotate(glm::mat4(1.0f), rot.y, glm::vec3(0,1,0));
        glm::mat4 rmz = glm::rotate(glm::mat4(1.0f), rot.z, glm::vec3(0,0,1));
        glm::mat4 sm = glm::scale(glm::mat4(1.0f), scale);
        mat = tm*(rmx*rmy*rmz)*sm;*/
        mat = ( glm::translate(glm::mat4(1.0f), pos) * ( glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1,0,0)) * glm::rotate(glm::mat4(1.0f), rot.y, glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1.0f), rot.z, glm::vec3(0,0,1)) ) * glm::scale(glm::mat4(1.0f), scale) );
    }

    inline bool IsAutoRecalc(){ return autoReCalc; }
    inline void SetAutoRecalc(const bool& state){
        autoReCalc = state;
        Calc();
    }

    inline void SetPos(const glm::vec3& p){
        if(pos!=p){
            pos = p;
            if(IsAutoRecalc()) Calc();
        }
    }

    inline void SetPos(glm::vec3* p){
        pos = *p;
        if(IsAutoRecalc()) Calc();
    }

    inline void SetRot(const glm::vec3& r){
        if(rot!=r){
            rot = r;
            if(IsAutoRecalc()) Calc();
        }
    }

    inline void SetRot(glm::vec3* r){
        rot = *r;
        if(IsAutoRecalc()) Calc();
    }

    inline void SetScale(const glm::vec3& s){
        if(scale!=s){
            scale = s;
            if(IsAutoRecalc()) Calc();
        }
    }

    inline void SetScale(glm::vec3* s){
        scale = *s;
        if(IsAutoRecalc()) Calc();
    }

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

    inline static glm::vec2 NormalizedDirection(float angle){
        return glm::vec2( glm::cos( glm::radians(angle) ), glm::sin( glm::radians(angle) ) );
    }

    inline static glm::vec3 NormalizedDirection(float angleX, float angleY, float angleZ){
        return glm::vec3(
            glm::cos( glm::radians(angleY) ) * glm::sin( glm::radians(angleX) ),
            glm::sin( glm::radians(angleY) ),
            glm::cos( glm::radians(angleY) ) * glm::cos( glm::radians(angleX) )
        );
    }

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
