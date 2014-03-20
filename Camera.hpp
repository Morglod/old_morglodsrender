#pragma once

#ifndef _MR_CAMERA_H_
#define _MR_CAMERA_H_

#include "Events.hpp"
#include "Transformation.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

namespace MR {

class RenderTarget;
class Camera {
public:
    enum class CameraMode : unsigned char {
        Target = 0, //camera looks at target point, camera rotates around target point
        Direction = 1 //camera looks at target point, target point rotates around camera
    };

    enum class CameraProjection : unsigned char {
        Perspective = 0,
        Ortho = 1
    };

    MR::Event<const CameraMode&> OnModeChanged;
    MR::Event<const CameraProjection&> OnProjectionChanged;
    MR::Event<const glm::vec3&> OnPositionChanged;
    MR::Event<const glm::vec3&> OnTargetChanged;
    MR::Event<const glm::vec3&> OnDirectionChanged;
    MR::Event<RenderTarget*> OnRenderTargetChanged;
    MR::Event<glm::mat4*> OnMVPRecalc;

    virtual void Use(const unsigned int & matrixUniform);
    inline bool IsAutoRecalc();

    inline CameraMode GetCameraMode();
    inline CameraProjection GetCameraProjection();

    inline glm::vec3 GetDirection();
    inline glm::vec3 GetLeftDirection();
    inline glm::vec3 GetUpDirection();

    inline glm::vec3* GetPosition();
    inline glm::vec3 GetTarget();
    inline glm::vec3 GetUp();

    inline float GetFovY();
    inline float* GetFovYP();
    inline float GetNearZ();
    inline float GetFarZ();
    inline float GetAspectRatio();
    inline float* GetAspectRatioP();

    inline glm::mat4* GetModelMatrix();
    inline glm::mat4* GetViewMatrix();
    inline glm::mat4* GetProjectMatrix();
    inline glm::mat4* GetInvModelViewMatrix();
    inline glm::mat4* GetMVP();

    inline MR::RenderTarget* GetRenderTarget();
    virtual void SetRenderTarget(MR::RenderTarget* rt);

    virtual void MoveForward(const glm::vec3& v);
    virtual void MoveLeft(const glm::vec3& v);
    virtual void MoveUp(const glm::vec3& v);

    virtual void Move(const glm::vec3& v);
    virtual void MoveTarget(const glm::vec3& t); //for target mode only

    virtual void Yaw(const float& v);
    virtual void Pitch(const float& v);
    virtual void Roll(const float& v);

    virtual void SetDirection(const glm::vec3& d, const glm::vec3& left_d, const glm::vec3& up_d);
    virtual void SetCameraMode(const CameraMode& cm);
    virtual void SetCameraProjection(const CameraProjection& cp);
    virtual void SetModelMatrix(glm::mat4* m);
    virtual void SetAutoRecalc(const bool& state);

    virtual void SetPosition(const glm::vec3& p);
    virtual void SetRotation(const glm::vec3& p);

    virtual void CalcViewMatrix();
    virtual void CalcProjectionMatrix();
    virtual void CalcMVP();
    virtual void Calc();

    Camera(const glm::vec3& camPos, const glm::vec3& camTarget, const float& fov, const float& nearZ, const float& farZ, const float& aspectR);
    virtual ~Camera();

protected:
    virtual void CalcDirections();

    CameraMode _cmode;
    CameraProjection _cproj;

    glm::vec3* _pos;
    glm::vec3 _rot;

    glm::vec3 _direction; //direction mode only
    glm::vec3 _left_direction; //direction mode only
    glm::vec3 _up_direction; //direction mode only
    glm::vec3 _target;

    glm::vec3 _up;

    float _fovY;
    float _zNear;
    float _zFar;
    float _aspectRatio;

    glm::mat4* _viewMatrix = new glm::mat4(1.0f);
    glm::mat4* _projectionMatrix = new glm::mat4(1.0f);
    glm::mat4* _modelMatrix = new glm::mat4(1.0f);
    glm::mat4* _inv_modelViewMatrix = new glm::mat4(1.0f);
    glm::mat4* _mvp = new glm::mat4(1.0f);

    bool _autoReCalc;

    MR::RenderTarget* _render_target;
private:
    Camera(const Camera&);
    Camera operator=(const Camera& c);
};
}

bool MR::Camera::IsAutoRecalc() {
    return _autoReCalc;
}

MR::Camera::CameraMode MR::Camera::GetCameraMode() {
    return _cmode;
}
MR::Camera::CameraProjection MR::Camera::GetCameraProjection() {
    return _cproj;
}

glm::vec3 MR::Camera::GetDirection() {
    return _direction;
}
glm::vec3 MR::Camera::GetLeftDirection(){
    return _left_direction;
}
glm::vec3 MR::Camera::GetUpDirection(){
    return _up_direction;
}

glm::vec3* MR::Camera::GetPosition() {
    return _pos;
}
glm::vec3 MR::Camera::GetTarget() {
    return _target;
}
glm::vec3 MR::Camera::GetUp() {
    return _up;
}

float MR::Camera::GetFovY() {
    return _fovY;
}
float* MR::Camera::GetFovYP() {
    return &_fovY;
}
float MR::Camera::GetNearZ() {
    return _zNear;
}
float MR::Camera::GetFarZ() {
    return _zFar;
}
float MR::Camera::GetAspectRatio() {
    return _aspectRatio;
}
float* MR::Camera::GetAspectRatioP() {
    return &_aspectRatio;
}

glm::mat4* MR::Camera::GetModelMatrix(){
    return _modelMatrix;
}

glm::mat4* MR::Camera::GetInvModelViewMatrix(){
    return _inv_modelViewMatrix;
}

glm::mat4* MR::Camera::GetViewMatrix() {
    return _viewMatrix;
}

glm::mat4* MR::Camera::GetProjectMatrix() {
    return _projectionMatrix;
}

glm::mat4* MR::Camera::GetMVP() {
    return _mvp;
}

MR::RenderTarget* MR::Camera::GetRenderTarget() {
    return _render_target;
}

#endif // _MR_CAMERA_H_
