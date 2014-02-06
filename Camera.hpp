#pragma once

#ifndef _MR_CAMERA_H_
#define _MR_CAMERA_H_

#include "pre.hpp"
#include "Events.hpp"
#include "Transformation.hpp"

namespace MR {

class RenderTarget;
class Camera {
public:
    enum CameraMode : unsigned char {
        CM_TARGET = 0, //camera looks at target point, camera rotates around target point
        CM_DIRECTION = 1 //camera looks at target point, target point rotates around camera
    };

    enum CameraProjection : unsigned char {
        CP_PERSPECTIVE = 0,
        CP_ORTHO = 1
    };

    MR::Event<const CameraMode&> OnModeChanged;
    MR::Event<const CameraProjection&> OnProjectionChanged;
    MR::Event<const glm::vec3&> OnPositionChanged;
    MR::Event<const glm::vec3&> OnTargetPosChanged; //only for target camera mode
    MR::Event<const glm::vec3&> OnDirectionChanged;
    MR::Event<RenderTarget*> OnRenderTargetChanged;
    MR::Event<glm::mat4*> OnMVPRecalc;

    virtual void Use(const GLuint& matrixUniform);
    inline bool IsAutoRecalc() const { return autoReCalc; }

    inline CameraMode GetCameraMode() const { return cmode; }
    inline CameraProjection GetCameraProjection() const { return cproj; }

    inline glm::vec3* GetDirectionP() const { return direction; }
    inline glm::vec3 GetCameraPosition() const { return pos; }
    inline glm::vec3 GetCameraTarget() const { return target; }
    inline glm::vec3 GetCameraUp() const { return up; }

    inline float GetFovY() const { return fovY; }
    inline float* GetFovYP() { return &fovY; }
    inline float GetNearZ() const { return zNear; }
    inline float GetFarZ() const { return zFar; }
    inline float GetAspectRatio() const { return aspectRatio; }
    inline float* GetAspectRatioP() { return &aspectRatio; }

    inline glm::mat4* GetViewMatrix() { return viewMatrix; }
    inline glm::mat4* GetProjectMatrix() { return projectionMatrix; }
    inline glm::mat4* GetMVP() { return mvp; }

    inline MR::RenderTarget* GetRenderTarget() { return _render_target; }
    virtual void SetRenderTarget(MR::RenderTarget* rt);

    virtual void MoveForward(const glm::vec3& v);
    virtual void MoveLeft(const glm::vec3& v);
    virtual void Move(const glm::vec3& v);
    virtual void MoveTarget(const glm::vec3& t);

    virtual void SetDirection(const glm::vec3& d);
    virtual void SetDirection(const glm::vec3& d, const glm::vec3 left_d);
    virtual void SetCameraMode(const CameraMode& cm);
    virtual void SetCameraProjection(const CameraProjection& cp);
    virtual void SetModelMatrix(glm::mat4* m);
    virtual void SetAutoRecalc(const bool& state);

    virtual void CalcViewMatrix();
    virtual void CalcProjectionMatrix();
    virtual void CalcMVP();
    virtual void Calc();

    Camera(glm::vec3 camPos, glm::vec3 camTarget, float fov, float nearZ, float farZ, float aspectR);
    virtual ~Camera();

    /*inline void Rotate(glm::vec3 v) {
        //if(camMode == CameraMode::Cam_TP) modelMatrix = glm::rotate(glm::rotate(glm::rotate(modelMatrix, v.x, glm::vec3(1,0,0)), v.y, glm::vec3(0,1,0)), v.z, glm::vec3(0,0,1));
    }*/

    /*inline void CalcModelMatrix(){
        modelMatrix = glm::translate(glm::rotate(glm::rotate(glm::rotate(glm::scale(glm::mat4(1.0f), ModelScale), ModelRot.x, glm::vec3(1, 0, 0)), ModelRot.y, glm::vec3(0,1,0)), ModelRot.z, glm::vec3(0,0,1)), ModelPos);
    }*/

protected:
    CameraMode cmode;
    CameraProjection cproj;

    glm::vec3 pos; //for target and direction mode
    glm::vec3* direction; //direction mode only
    glm::vec3* left_direction; //direction mode only
    glm::vec3 target; //target mode only

    glm::vec3 up;

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

    MR::RenderTarget* _render_target;
};
}

#endif // _MR_CAMERA_H_
