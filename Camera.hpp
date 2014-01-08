#pragma once

#ifndef _MR_CAMERA_H_
#define _MR_CAMERA_H_

#include "MorglodsRender.hpp"

namespace MR {
class Camera {
public:
    enum CameraMode : unsigned char {
        CM_TARGET = 0,
        CM_DIRECTION = 1,
        CM_NUM = 3
    };
protected:
    CameraMode cmode;

    //target and direction mode
    glm::vec3 pos;

    //direction mode only
    glm::vec3 direction;

    //target mode only
    glm::vec3 target;

    //up vector
    glm::vec3 up;

    /*glm::vec3 modelPos;
    glm::vec3 modelRot;
    glm::vec3 modelScale;
    glm::mat4 modelMatrix;*/

    float fovY;
    float zNear;
    float zFar;
    float aspectRatio;

    glm::mat4* viewMatrix;
    glm::mat4* projectionMatrix;
    glm::mat4* mvp;

    bool autoReCalc;

public:
    inline bool IsAutoRecalc() {
        return autoReCalc;
    }

    inline static glm::vec3 WorldForwardVector() {
        return glm::vec3(0,0,-1);
    }

    inline static glm::vec3 WorldBackwardVector() {
        return glm::vec3(0,0,1);
    }

    inline static glm::vec3 WorldUpVector() {
        return glm::vec3(0,1,0);
    }

    inline static glm::vec3 WorldDownVector() {
        return glm::vec3(0,-1,0);
    }

    inline static glm::vec3 WorldLeftVector() {
        return glm::vec3(-1,0,0);
    }

    inline static glm::vec3 WorldRightVector() {
        return glm::vec3(1,0,0);
    }

    void Use(GLuint matrixUniform);

    inline void Move(glm::vec3 v) {
        //if(cmode == CameraMode::CM_TARGET) modelMatrix = glm::translate(modelMatrix, v);
        if(cmode == CameraMode::CM_TARGET) {
            pos += v;
            if(IsAutoRecalc()) calcViewMatrix();
        }//viewMatrix = glm::translate(viewMatrix, v);
    }

    inline void MoveTarget(glm::vec3 t) {
        if(cmode == CameraMode::CM_TARGET) {
            target += t;
            if(IsAutoRecalc()) calcViewMatrix();
        }
    }

    /*inline void Rotate(glm::vec3 v) {
        //if(camMode == CameraMode::Cam_TP) modelMatrix = glm::rotate(glm::rotate(glm::rotate(modelMatrix, v.x, glm::vec3(1,0,0)), v.y, glm::vec3(0,1,0)), v.z, glm::vec3(0,0,1));
    }*/

    inline CameraMode GetCameraMode() const {
        return cmode;
    }

    inline glm::vec3 GetCameraPosition() const {
        return pos;
    }

    inline glm::vec3 GetCameraTarget() const {
        return target;
    }

    inline glm::vec3 GetCameraUp() const {
        return up;
    }

    inline float GetFovY() const {
        return fovY;
    }

    inline float GetNearZ() const {
        return zNear;
    }

    inline float GetFarZ() const {
        return zFar;
    }

    inline float GetAspectRatio() const {
        return aspectRatio;
    }

    inline void calcViewMatrix() {
        if(cmode == CameraMode::CM_TARGET) *viewMatrix = glm::lookAt(pos, target, up);
    }

    /*inline void CalcModelMatrix(){
        modelMatrix = glm::translate(glm::rotate(glm::rotate(glm::rotate(glm::scale(glm::mat4(1.0f), ModelScale), ModelRot.x, glm::vec3(1, 0, 0)), ModelRot.y, glm::vec3(0,1,0)), ModelRot.z, glm::vec3(0,0,1)), ModelPos);
    }*/

    inline void calcProjectionMatrix() {
        *projectionMatrix = glm::perspective(fovY, aspectRatio, zNear, zFar);
    }

    inline void calcMVP() {
        *mvp = (*projectionMatrix)*(*viewMatrix)/*modelMatrix*/;
    }

    inline void calc() {
        calcViewMatrix();
        //CalcModelMatrix();
        calcProjectionMatrix();
        calcMVP();
    }

    inline glm::mat4* GetViewMatrix() {
        //CalcViewMatrix();
        return viewMatrix;
    }

    /*inline glm::mat4 GetModelMatrix() {
        CalcModelMatrix();
        return modelMatrix;
    }*/

    inline glm::mat4* GetProjectMatrix() {
        //CalcProjectionMatrix();
        return projectionMatrix;
    }

    inline glm::mat4* GetMVP() {
        calcMVP();
        return mvp;
    }

    inline void SetAutoRecalc(bool state) {
        autoReCalc = state;
        calc();
    }

    inline Camera(glm::vec3 camPos, glm::vec3 camTarget, float fov, float nearZ, float farZ, float aspectR)
        : cmode(CameraMode::CM_TARGET), pos(camPos), direction( WorldForwardVector() ), target(camTarget), up(glm::vec3(0,1,0)),
          fovY(fov), zNear(nearZ), zFar(farZ), aspectRatio(aspectR), autoReCalc(false) {

        //allocate space, functionality like calc()
        viewMatrix = new glm::mat4(glm::lookAt(pos, target, up));
        projectionMatrix = new glm::mat4(glm::perspective(fovY, aspectRatio, zNear, zFar));
        mvp = new glm::mat4( (*projectionMatrix) * (*viewMatrix) );
    }
};
}

#endif // _MR_CAMERA_H_
