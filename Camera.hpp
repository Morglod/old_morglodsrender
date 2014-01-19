#pragma once

#ifndef _MR_CAMERA_H_
#define _MR_CAMERA_H_

#include "pre.hpp"
#include "Events.hpp"

namespace MR {
class Camera {
public:
    enum CameraMode : unsigned char {
        CM_TARGET = 0,
        CM_DIRECTION = 1
    };

    enum CameraProjection : unsigned char {
        CP_PERSPECTIVE = 0,
        CP_ORTHO = 1
    };
protected:
    CameraMode cmode;
    CameraProjection cproj;

    //target and direction mode
    glm::vec3 pos;

    //direction mode only
    glm::vec3* direction;

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

    float ortho_left, ortho_right, ortho_top, ortho_bottom;

    glm::mat4* viewMatrix;
    glm::mat4* projectionMatrix;
    glm::mat4* modelMatrix;
    glm::mat4* mvp;

    bool autoReCalc;

public:
    /** sender - Camera
    */
    MR::Event<const CameraProjection&> OnProjectionChanged;

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

    inline void MoveForward(glm::vec3 v) {
        //if(cmode == CameraMode::CM_TARGET) modelMatrix = glm::translate(modelMatrix, v);
        //if(cmode == CameraMode::CM_TARGET) {
            pos += v * (*direction);
            if(IsAutoRecalc()) calcViewMatrix();
        //}//viewMatrix = glm::translate(viewMatrix, v);
    }

    inline void Move(glm::vec3 v) {
        //if(cmode == CameraMode::CM_TARGET) modelMatrix = glm::translate(modelMatrix, v);
        //if(cmode == CameraMode::CM_TARGET) {
            pos += v;
            if(IsAutoRecalc()) calcViewMatrix();
        //}//viewMatrix = glm::translate(viewMatrix, v);
    }

    inline void MoveTarget(glm::vec3 t) {
        if(cmode == CameraMode::CM_TARGET) {
            target += t;
            if(IsAutoRecalc()) calcViewMatrix();
        }
    }

    inline void SetDirection(glm::vec3 d) {
        *direction = d;
        if(IsAutoRecalc()) calcViewMatrix();
    }

    inline glm::vec3* GetDirectionP() {
        return direction;
    }

    /*inline void Rotate(glm::vec3 v) {
        //if(camMode == CameraMode::Cam_TP) modelMatrix = glm::rotate(glm::rotate(glm::rotate(modelMatrix, v.x, glm::vec3(1,0,0)), v.y, glm::vec3(0,1,0)), v.z, glm::vec3(0,0,1));
    }*/

    inline CameraMode GetCameraMode() const {
        return cmode;
    }

    inline void SetCameraMode(CameraMode cm) {
        cmode = cm;
    }

    inline CameraProjection GetCameraProjection() const {
        return cproj;
    }

    inline void SetCameraProjection(const CameraProjection& cp) {
        if(cproj != cp){
            cproj = cp;
            OnProjectionChanged(this, cp);
        }
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

    inline float* GetFovYP() {
        return &fovY;
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

    inline float* GetAspectRatioP() {
        return &aspectRatio;
    }

    inline void calcViewMatrix() {
        if(cmode == CameraMode::CM_TARGET) *viewMatrix = glm::lookAt(pos, target, up);
        if(cmode == CameraMode::CM_DIRECTION) *viewMatrix = glm::lookAt(pos, pos + (*direction), up);
    }

    /*inline void CalcModelMatrix(){
        modelMatrix = glm::translate(glm::rotate(glm::rotate(glm::rotate(glm::scale(glm::mat4(1.0f), ModelScale), ModelRot.x, glm::vec3(1, 0, 0)), ModelRot.y, glm::vec3(0,1,0)), ModelRot.z, glm::vec3(0,0,1)), ModelPos);
    }*/

    inline void calcProjectionMatrix() {
        if(cproj == CameraProjection::CP_PERSPECTIVE) *projectionMatrix = glm::perspective(fovY, aspectRatio, zNear, zFar);
        if(cproj == CameraProjection::CP_ORTHO) *projectionMatrix = glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, zNear, zFar);
    }

    inline void calcMVP() {
        if(modelMatrix) *mvp = (*projectionMatrix)*(*viewMatrix)*(*modelMatrix);
        else *mvp = (*projectionMatrix)*(*viewMatrix);
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

    inline void SetModelMatrix(glm::mat4* m) {
        modelMatrix = m;
        if(autoReCalc) {
            calcMVP();
        }
    }

    inline glm::mat4* GetProjectMatrix() {
        //CalcProjectionMatrix();
        return projectionMatrix;
    }

    inline glm::mat4* GetMVP() {
        //calcMVP();
        return mvp;
    }

    inline void SetAutoRecalc(bool state) {
        autoReCalc = state;
        if(state) calc();
    }

    inline Camera(glm::vec3 camPos, glm::vec3 camTarget, float fov, float nearZ, float farZ, float aspectR)
        : cmode(CameraMode::CM_TARGET), pos(camPos), direction( new glm::vec3(WorldForwardVector()) ), target(camTarget), up(glm::vec3(0,1,0)),
          fovY(fov), zNear(nearZ), zFar(farZ), aspectRatio(aspectR), ortho_left(0.0f), ortho_right(100.0f), ortho_top(0.0f), ortho_bottom(100.0f), modelMatrix(nullptr), autoReCalc(false) {

        //allocate space, functionality like calc()
        viewMatrix = new glm::mat4(glm::lookAt(pos, target, up));
        projectionMatrix = new glm::mat4(glm::perspective(fovY, aspectRatio, zNear, zFar));
        mvp = new glm::mat4( (*projectionMatrix) * (*viewMatrix) );
    }
};
}

#endif // _MR_CAMERA_H_
