#pragma once

#ifndef _MR_CAMERA_H_
#define _MR_CAMERA_H_

#include "../Utils/Events.hpp"
#include "Transformation.hpp"
#include "../Types.hpp"
#include "SceneInterfaces.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

namespace MR {

class IShaderProgram;
class IShaderUniform;

class Camera : public ICamera {
public:
    void AttachToShader(IShaderProgram* shader) override;
    void UpdateShader(IShaderProgram* shader) override;

    inline bool IsAutoRecalc() override;

    inline CameraMode GetCameraMode() override;
    inline CameraProjection GetCameraProjection() override;

    inline glm::vec3 GetForwardDirection() override;
    inline glm::vec3 GetLeftDirection() override;
    inline glm::vec3 GetUpDirection() override;

    inline glm::vec3* GetPosition() override;
    inline glm::vec3 GetTarget() override;
    inline glm::vec3 GetUp() override;

    inline float GetFovY() override;
    inline float* GetFovYP() override;
    inline float GetNearZ() override;
    inline float GetFarZ() override;
    inline float GetAspectRatio() override;
    inline float* GetAspectRatioP() override;

    inline glm::mat4* GetModelMatrix() override;
    inline glm::mat4* GetViewMatrix() override;
    inline glm::mat4* GetProjectMatrix() override;
    inline glm::mat4* GetInvModelViewMatrix() override;
    inline glm::mat4* GetMVP() override;

    inline MR::RenderTarget* GetRenderTarget();
    void SetRenderTarget(MR::RenderTarget* rt);

    void MoveForward(const glm::vec3& v) override;
    void MoveLeft(const glm::vec3& v) override;
    void MoveUp(const glm::vec3& v) override;

    void Move(const glm::vec3& v) override;
    void MoveTarget(const glm::vec3& t) override; //for target mode only

    void Yaw(const float& v) override;
    void Pitch(const float& v) override;
    void Roll(const float& v) override;

    void SetDirection(const glm::vec3& d, const glm::vec3& left_d, const glm::vec3& up_d) override;
    void SetCameraMode(const CameraMode& cm) override;
    void SetCameraProjection(const CameraProjection& cp) override;
    void SetModelMatrix(glm::mat4* m) override;
    void SetAutoRecalc(const bool& state) override;

    void SetPosition(const glm::vec3& p) override;
    void SetRotation(const glm::vec3& p) override;

    void CalcViewMatrix() override;
    void CalcProjectionMatrix() override;
    void CalcMVP() override;
    void Calc() override;

    Camera(const glm::vec3& camPos, const glm::vec3& camTarget, const float& fov, const float& nearZ, const float& farZ, const float& aspectR);
    virtual ~Camera();

protected:
    virtual void CalcDirections();

    CameraMode _cmode;
    CameraProjection _cproj;

    glm::vec3* _pos;
    glm::vec3 _rot;

    glm::vec3 _forward_direction; //direction mode only
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

    class AttachedShader {
    public:
        IShaderProgram* _shader = nullptr;
        IShaderUniform* _viewUniform = nullptr;
        IShaderUniform* _projUniform = nullptr;
        IShaderUniform* _modelUniform = nullptr;
        IShaderUniform* _mvpUniform = nullptr;
        IShaderUniform* _posUniform = nullptr;
        IShaderUniform* _dirUniform = nullptr;
    };

    std::vector<AttachedShader> _attachedShaders;

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

glm::vec3 MR::Camera::GetForwardDirection() {
    return _forward_direction;
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
