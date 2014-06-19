#include "Camera.hpp"
#include "RenderTarget.hpp"
#include "Shader.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#include <glm/gtc/matrix_transform.hpp>

void MR::Camera::AttachToShader(IShader* shader) {
    for(size_t i = 0; i < _attachedShaders.size(); ++i){
        if(_attachedShaders[i]._shader == shader) return;
    }

    if(shader){
        AttachedShader as;
        as._shader = shader;

        IShaderUniform* su = shader->FindShaderUniform(MR_SHADER_MVP_MAT4);
        if(su) {
            as._mvpUniform = su;
            as._mvpUniform->SetValue(&(*_mvp)[0][0]);
        }
        else as._mvpUniform = shader->CreateUniform(MR_SHADER_MVP_MAT4, IShaderUniform::Types::Mat4, &(*_mvp)[0][0]);

        su = shader->FindShaderUniform(MR_SHADER_MODEL_MAT4);
        if(su) {
            as._modelUniform = su;
            as._modelUniform->SetValue(&(*_modelMatrix)[0][0]);
        }
        else as._modelUniform = shader->CreateUniform(MR_SHADER_MODEL_MAT4, IShaderUniform::Types::Mat4, &(*_modelMatrix)[0][0]);

        su = shader->FindShaderUniform(MR_SHADER_VIEW_MAT4);
        if(su) {
            as._viewUniform = su;
            as._viewUniform->SetValue(&(*_viewMatrix)[0][0]);
        }
        else as._viewUniform = shader->CreateUniform(MR_SHADER_VIEW_MAT4, IShaderUniform::Types::Mat4, &(*_viewMatrix)[0][0]);

        su = shader->FindShaderUniform(MR_SHADER_PROJ_MAT4);
        if(su) {
            as._projUniform = su;
            as._projUniform->SetValue(&(*_projectionMatrix)[0][0]);
        }
        else as._projUniform = shader->CreateUniform(MR_SHADER_PROJ_MAT4, IShaderUniform::Types::Mat4, &(*_projectionMatrix)[0][0]);

        su = shader->FindShaderUniform(MR_SHADER_CAM_POS);
        if(su) {
            as._posUniform = su;
            as._posUniform->SetValue(&(*_pos));
        }
        else as._posUniform = shader->CreateUniform(MR_SHADER_CAM_POS, MR::IShaderUniform::Types::Vec3, &(*_pos));

        su = shader->FindShaderUniform(MR_SHADER_CAM_DIR);
        if(su) {
            as._dirUniform = su;
            as._dirUniform->SetValue(&_direction);
        }
        else as._dirUniform = shader->CreateUniform(MR_SHADER_CAM_DIR, MR::IShaderUniform::Types::Vec3, &_direction);

        _attachedShaders.push_back(as);
    }
    //glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &(*_mvp)[0][0]);
}

void MR::Camera::UpdateShader(IShader* shader) {
    shader->SetUniform(MR_SHADER_MVP_MAT4, IShaderUniform::Types::Mat4, &(*_mvp)[0][0]);
    shader->SetUniform(MR_SHADER_MODEL_MAT4, IShaderUniform::Types::Mat4, &(*_modelMatrix)[0][0]);
    shader->SetUniform(MR_SHADER_VIEW_MAT4, IShaderUniform::Types::Mat4, &(*_viewMatrix)[0][0]);
    shader->SetUniform(MR_SHADER_PROJ_MAT4, IShaderUniform::Types::Mat4, &(*_projectionMatrix)[0][0]);
    shader->SetUniform(MR_SHADER_CAM_POS, MR::IShaderUniform::Types::Vec3, &(*_pos));
    shader->SetUniform(MR_SHADER_CAM_DIR, MR::IShaderUniform::Types::Vec3, &_direction);
}

void MR::Camera::MoveForward(const glm::vec3& v) {
    if(v == glm::vec3(0,0,0)) return;
    *_pos += v * _direction;
    if(IsAutoRecalc()) CalcViewMatrix();
    OnPositionChanged(this, *_pos);
}

void MR::Camera::MoveLeft(const glm::vec3& v) {
    if(v == glm::vec3(0,0,0)) return;
    *_pos += v * _left_direction;
    if(IsAutoRecalc()) CalcViewMatrix();
    OnPositionChanged(this, *_pos);
}

void MR::Camera::MoveUp(const glm::vec3& v){
    if(v == glm::vec3(0,0,0)) return;
    *_pos += v * _up_direction;
    if(IsAutoRecalc()) CalcViewMatrix();
    OnPositionChanged(this, *_pos);
}

void MR::Camera::Move(const glm::vec3& v) {
    if(v == glm::vec3(0,0,0)) return;
    *_pos += v;
    if(IsAutoRecalc()) CalcViewMatrix();
    OnPositionChanged(this, *_pos);
}

void MR::Camera::MoveTarget(const glm::vec3& t) {
    if( t == glm::vec3(0,0,0) ) return;
    if(_cmode == MR::Camera::CameraMode::Target) {
        _target += t;
        if(IsAutoRecalc()) CalcViewMatrix();
        OnTargetChanged(this, _target);
    }
}

void MR::Camera::Yaw(const float& v){
    if(v == 0.0f) return;
    _rot.y += -v;
    CalcDirections();
}

void MR::Camera::Pitch(const float& v){
    if(v == 0.0f) return;
    _rot.z += v;
    CalcDirections();
}

void MR::Camera::Roll(const float& v){
    if(v == 0.0f) return;
    _rot.x += v;
    CalcDirections();
}

void MR::Camera::SetDirection(const glm::vec3& d, const glm::vec3& left_d, const glm::vec3& up_d) {
    if( _direction != d ) {
        _direction = d;
        _left_direction = left_d;
        _up_direction = up_d;
        if(IsAutoRecalc()) CalcViewMatrix();
        OnDirectionChanged(this, d);
    }
}

void MR::Camera::SetCameraMode(const CameraMode& cm) {
    if(_cmode != cm) {
        _cmode = cm;
        OnModeChanged(this, cm);
    }
}

void MR::Camera::SetCameraProjection(const CameraProjection& cp) {
    if(_cproj != cp) {
        _cproj = cp;
        OnProjectionChanged(this, cp);
    }
}

void MR::Camera::CalcViewMatrix() {
    if(_cmode == CameraMode::Target) *_viewMatrix = glm::lookAt(*_pos, _target, _up_direction);
    if(_cmode == CameraMode::Direction) *_viewMatrix = glm::lookAt(*_pos, *_pos + _direction, _up_direction);
}

void MR::Camera::CalcProjectionMatrix() {
    if(_cproj == CameraProjection::Perspective) *_projectionMatrix = glm::perspective(_fovY, _aspectRatio, _zNear, _zFar);
    //if(_cproj == CameraProjection::Ortho) *_projectionMatrix = glm::ortho(_ortho_left, _ortho_right, _ortho_bottom, _ortho_top, _zNear, _zFar);
}

void MR::Camera::CalcMVP() {
    if(_modelMatrix) {
        *_mvp = (*_projectionMatrix)*(*_viewMatrix)*(*_modelMatrix);
        *_inv_modelViewMatrix = glm::transpose( glm::inverse( (*_viewMatrix)*(*_modelMatrix) ) );
    }
    else *_mvp = (*_projectionMatrix)*(*_viewMatrix);
    OnMVPRecalc(this, _mvp);
}

void MR::Camera::Calc() {
    CalcViewMatrix();
    CalcProjectionMatrix();
    CalcMVP();
}

void MR::Camera::SetModelMatrix(glm::mat4* m) {
    *_modelMatrix = *m;
    if(_autoReCalc) {
        CalcMVP();
    }
}

void MR::Camera::SetAutoRecalc(const bool& state) {
    _autoReCalc = state;
    if(state) Calc();
}

void MR::Camera::SetRenderTarget(MR::RenderTarget* renderTarget){
    if(_render_target != renderTarget){
        _render_target = renderTarget;
        OnRenderTargetChanged(this, renderTarget);
    }
}

void MR::Camera::SetPosition(const glm::vec3& p){
    *_pos = p;
    if(_autoReCalc) Calc();
}

void MR::Camera::SetRotation(const glm::vec3& p){
    _rot = p;
    if(_autoReCalc) CalcDirections();
}

MR::Camera::Camera(const glm::vec3& camPos, const glm::vec3& camTarget, const float& fov, const float& nearZ, const float& farZ, const float& aspectR)
    : _cmode(CameraMode::Target), _cproj(CameraProjection::Perspective), _pos( new glm::vec3(camPos) ), _direction(MR::Transform::WorldForwardVector()), _left_direction(MR::Transform::WorldLeftVector()), _up_direction(MR::Transform::WorldUpVector()), _target(camTarget), _up(glm::vec3(0,1,0)),
      _fovY(fov), _zNear(nearZ), _zFar(farZ), _aspectRatio(aspectR), _modelMatrix( new glm::mat4(1.0f) ), _autoReCalc(false), _render_target(nullptr) {

    //allocate space, functionality like calc()
    _viewMatrix = new glm::mat4(glm::lookAt(*_pos, _target, _up));
    _projectionMatrix = new glm::mat4(glm::perspective(_fovY, _aspectRatio, _zNear, _zFar));
    _mvp = new glm::mat4( (*_projectionMatrix) * (*_viewMatrix) );
}

MR::Camera::~Camera() {
    delete _pos;
    delete _viewMatrix;
    delete _projectionMatrix;
    delete _modelMatrix;
    delete _mvp;
}

MR::Camera::Camera(const MR::Camera&)
    : _cmode(CameraMode::Target), _cproj(CameraProjection::Perspective), _pos(), _direction(), _left_direction(), _target(), _up(glm::vec3(0,1,0)),
      _fovY(90.0f), _zNear(0.0f), _zFar(100.0f), _aspectRatio(1.0f), _modelMatrix(nullptr), _autoReCalc(false), _render_target(nullptr) {
}

void MR::Camera::CalcDirections(){
    if(_cmode == CameraMode::Direction){
        _direction = MR::Transform::NormalizedDirection(_rot.x, _rot.y, _rot.z);
        _up_direction = MR::Transform::NormalizedDirection(_rot.x, _rot.y-90.0f, _rot.z);
        _left_direction = MR::Transform::NormalizedDirection(_rot.x-90.0f, 0.0f, _rot.z);

        if(_autoReCalc){
            CalcViewMatrix();
        }
    }
}

MR::Camera MR::Camera::operator=(const MR::Camera& c){
    return c;
}
