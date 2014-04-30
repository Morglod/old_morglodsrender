#pragma once

#ifndef _MR_CAMERA_H_
#define _MR_CAMERA_H_

#include "Events.hpp"
#include "Transformation.hpp"
#include "Types.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

namespace MR {

class IShader;
class IShaderUniform;
class RenderTarget;

class ICamera {
public:
    enum class CameraMode : unsigned char {
        Target = 0, //camera looks at target point, camera rotates around target point
        Direction = 1 //camera looks at target point, target point rotates around camera
    };

    enum class CameraProjection : unsigned char {
        Perspective = 0,
        Ortho = 1
    };

    MR::EventListener<ICamera*, const CameraMode&> OnModeChanged;
    MR::EventListener<ICamera*, const CameraProjection&> OnProjectionChanged;
    MR::EventListener<ICamera*, const glm::vec3&> OnPositionChanged;
    MR::EventListener<ICamera*, const glm::vec3&> OnTargetChanged;
    MR::EventListener<ICamera*, const glm::vec3&> OnDirectionChanged;
    MR::EventListener<ICamera*, RenderTarget*> OnRenderTargetChanged;
    MR::EventListener<ICamera*, glm::mat4*> OnMVPRecalc;

    virtual void AttachToShader(IShader* shader) = 0;

    virtual void SetAutoRecalc(const bool& state) = 0;
    virtual bool IsAutoRecalc() = 0;

    virtual CameraMode GetCameraMode() = 0;
    virtual CameraProjection GetCameraProjection() = 0;

    virtual glm::vec3 GetDirection() = 0;
    virtual glm::vec3 GetLeftDirection() = 0;
    virtual glm::vec3 GetUpDirection() = 0;

    virtual glm::vec3* GetPosition() = 0;
    virtual glm::vec3 GetTarget() = 0;
    virtual glm::vec3 GetUp() = 0;

    virtual float GetFovY() = 0;
    virtual float* GetFovYP() = 0;
    virtual float GetNearZ() = 0;
    virtual float GetFarZ() = 0;
    virtual float GetAspectRatio() = 0;
    virtual float* GetAspectRatioP() = 0;

    virtual glm::mat4* GetModelMatrix() = 0;
    virtual glm::mat4* GetViewMatrix() = 0;
    virtual glm::mat4* GetProjectMatrix() = 0;
    virtual glm::mat4* GetInvModelViewMatrix() = 0;
    virtual glm::mat4* GetMVP() = 0;

    virtual MR::RenderTarget* GetRenderTarget() = 0;
    virtual void SetRenderTarget(MR::RenderTarget* rt) = 0;

    virtual void MoveForward(const glm::vec3& v) = 0;
    virtual void MoveLeft(const glm::vec3& v) = 0;
    virtual void MoveUp(const glm::vec3& v) = 0;

    virtual void Move(const glm::vec3& v) = 0;
    virtual void MoveTarget(const glm::vec3& t) = 0; //for target mode only

    virtual void Yaw(const float& v) = 0;
    virtual void Pitch(const float& v) = 0;
    virtual void Roll(const float& v) = 0;

    virtual void SetDirection(const glm::vec3& d, const glm::vec3& left_d, const glm::vec3& up_d) = 0;
    virtual void SetCameraMode(const CameraMode& cm) = 0;
    virtual void SetCameraProjection(const CameraProjection& cp) = 0;
    virtual void SetModelMatrix(glm::mat4* m) = 0;

    virtual void SetPosition(const glm::vec3& p) = 0;
    virtual void SetRotation(const glm::vec3& p) = 0;

    virtual void CalcViewMatrix() = 0;
    virtual void CalcProjectionMatrix() = 0;
    virtual void CalcMVP() = 0;
    virtual void Calc() = 0;

    virtual ~ICamera() {}
};

class Camera : public MR::Object, public ICamera {
public:
    void AttachToShader(IShader* shader) override;
    inline bool IsAutoRecalc() override;

    inline CameraMode GetCameraMode() override;
    inline CameraProjection GetCameraProjection() override;

    inline glm::vec3 GetDirection() override;
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

    inline MR::RenderTarget* GetRenderTarget() override;
    void SetRenderTarget(MR::RenderTarget* rt) override;

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

    class AttachedShader {
    public:
        IShader* _shader = nullptr;
        IShaderUniform* _viewUniform = nullptr;
        IShaderUniform* _projUniform = nullptr;
        IShaderUniform* _modelUniform = nullptr;
        IShaderUniform* _mvpUniform = nullptr;
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
