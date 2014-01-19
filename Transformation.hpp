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
    inline void calc(){
        /*mat = glm::translate(
                             glm::rotate(
                                         glm::rotate(
                                                     glm::rotate(
                                                                 glm::scale(glm::mat4(1.0f), scale),
                                                                 rot.x, glm::vec3(1,0,0)),
                                                     rot.y, glm::vec3(0,1,0)
                                                     ),
                                         rot.z, glm::vec3(0,0,1)
                                         ),
                             pos);*/
        //mat = glm::scale()
        glm::mat4 tm = glm::translate(glm::mat4(1.0f), pos);
        glm::mat4 rmx = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1,0,0));
        glm::mat4 rmy = glm::rotate(glm::mat4(1.0f), rot.y, glm::vec3(0,1,0));
        glm::mat4 rmz = glm::rotate(glm::mat4(1.0f), rot.z, glm::vec3(0,0,1));
        glm::mat4 sm = glm::scale(glm::mat4(1.0f), scale);
        mat = tm*(rmx*rmy*rmz)*sm;
    }

    inline bool IsAutoRecalc(){
        return autoReCalc;
    }

    inline void SetAutoRecalc(bool state){
        autoReCalc = state;
        calc();
    }

    inline void setPos(glm::vec3& p){
        pos = p;
        if(IsAutoRecalc()) calc();
    }

    inline void setPos(glm::vec3* p){
        pos = *p;
        if(IsAutoRecalc()) calc();
    }

    inline void setRot(glm::vec3& r){
        rot = r;
        if(IsAutoRecalc()) calc();
    }

    inline void setRot(glm::vec3* r){
        rot = *r;
        if(IsAutoRecalc()) calc();
    }

    inline void setScale(glm::vec3& s){
        scale = s;
        if(IsAutoRecalc()) calc();
    }

    inline void setScale(glm::vec3* s){
        scale = *s;
        if(IsAutoRecalc()) calc();
    }

    inline glm::vec3& getPos(){
        return pos;
    }

    inline glm::vec3& getRot(){
        return rot;
    }

    inline glm::vec3* getRotP(){
        return &rot;
    }

    inline glm::vec3& getScale(){
        return scale;
    }

    inline glm::mat4& getMat(){
        return mat;
    }

    inline Transform() : mat(glm::mat4(1.0f)), pos(glm::vec3(0.0f)), rot(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), autoReCalc(true) {
        calc();
    }

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

    inline Transform(glm::mat4 m) : mat(m), pos(glm::vec3(0.0f)), rot(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), autoReCalc(true) {}
};
}

#endif
