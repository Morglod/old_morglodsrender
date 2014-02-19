#pragma once

#ifndef _MR_CAMERA_H_
#define _MR_CAMERA_H_

#include "Events.hpp"
#include "Transformation.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

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

    virtual void Use(const unsigned int & matrixUniform);
    inline bool IsAutoRecalc() const { return _autoReCalc; }

    inline CameraMode GetCameraMode() const { return _cmode; }
    inline CameraProjection GetCameraProjection() const { return _cproj; }

    inline glm::vec3* GetDirectionP() const { return _direction; }
    inline glm::vec3 GetCameraPosition() const { return _pos; }
    inline glm::vec3 GetCameraTarget() const { return _target; }
    inline glm::vec3 GetCameraUp() const { return _up; }

    inline float GetFovY() const { return _fovY; }
    inline float* GetFovYP() { return &_fovY; }
    inline float GetNearZ() const { return _zNear; }
    inline float GetFarZ() const { return _zFar; }
    inline float GetAspectRatio() const { return _aspectRatio; }
    inline float* GetAspectRatioP() { return &_aspectRatio; }

    inline glm::mat4* GetViewMatrix() { return _viewMatrix; }
    inline glm::mat4* GetProjectMatrix() { return _projectionMatrix; }
    inline glm::mat4* GetMVP() { return _mvp; }

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

    Camera(const glm::vec3& camPos, const glm::vec3& camTarget, const float& fov, const float& nearZ, const float& farZ, const float& aspectR);
    virtual ~Camera();

protected:
    CameraMode _cmode;
    CameraProjection _cproj;

    glm::vec3 _pos; //for target and direction mode
    glm::vec3* _direction; //direction mode only
    glm::vec3* _left_direction; //direction mode only
    glm::vec3 _target; //target mode only

    glm::vec3 _up;

    float _fovY;
    float _zNear;
    float _zFar;
    float _aspectRatio;
    float _ortho_left, _ortho_right, _ortho_top, _ortho_bottom;

    glm::mat4* _viewMatrix;
    glm::mat4* _projectionMatrix;
    glm::mat4* _modelMatrix;
    glm::mat4* _mvp;

    bool _autoReCalc;

    MR::RenderTarget* _render_target;
private:
    Camera(const Camera&);
    Camera operator=(const Camera& c);
};
}

#endif // _MR_CAMERA_H_
