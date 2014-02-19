#include "Camera.hpp"
#include "RenderTarget.hpp"

void MR::Camera::Use(const GLuint& matrixUniform) {
    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &(*_mvp)[0][0]);
}

void MR::Camera::MoveForward(const glm::vec3& v) {
    if(v == glm::vec3(0,0,0)) return;
    _pos += v * (*_direction);
    if(IsAutoRecalc()) CalcViewMatrix();
    OnPositionChanged(this, _pos);
}

void MR::Camera::MoveLeft(const glm::vec3& v) {
    if(v == glm::vec3(0,0,0)) return;
    _pos += v * (*_left_direction);
    if(IsAutoRecalc()) CalcViewMatrix();
    OnPositionChanged(this, _pos);
}

void MR::Camera::Move(const glm::vec3& v) {
    if(v == glm::vec3(0,0,0)) return;
    _pos += v;
    if(IsAutoRecalc()) CalcViewMatrix();
    OnPositionChanged(this, _pos);
}

void MR::Camera::MoveTarget(const glm::vec3& t) {
    if( (_cmode == CameraMode::CM_TARGET) && (t != glm::vec3(0,0,0)) ) {
        _target += t;
        if(IsAutoRecalc()) CalcViewMatrix();
        OnTargetPosChanged(this, _target);
    }
}

void MR::Camera::SetDirection(const glm::vec3& d) {
    if( (*_direction) != d ) {
        *_direction = d;
        //*left_direction = left_d;
        if(IsAutoRecalc()) CalcViewMatrix();
        OnDirectionChanged(this, d);
    }
}

void MR::Camera::SetDirection(const glm::vec3& d, const glm::vec3 left_d) {
    if( (*_direction) != d ) {
        *_direction = d;
        *_left_direction = left_d;
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
    if(_cmode == CameraMode::CM_TARGET) *_viewMatrix = glm::lookAt(_pos, _target, _up);
    if(_cmode == CameraMode::CM_DIRECTION) *_viewMatrix = glm::lookAt(_pos, _pos + (*_direction), _up);
}

void MR::Camera::CalcProjectionMatrix() {
    if(_cproj == CameraProjection::CP_PERSPECTIVE) *_projectionMatrix = glm::perspective(_fovY, _aspectRatio, _zNear, _zFar);
    if(_cproj == CameraProjection::CP_ORTHO) *_projectionMatrix = glm::ortho(_ortho_left, _ortho_right, _ortho_bottom, _ortho_top, _zNear, _zFar);
}

void MR::Camera::CalcMVP() {
    if(_modelMatrix) *_mvp = (*_projectionMatrix)*(*_viewMatrix)*(*_modelMatrix);
    else *_mvp = (*_projectionMatrix)*(*_viewMatrix);
    OnMVPRecalc(this, _mvp);
}

void MR::Camera::Calc() {
#ifdef MR_USE_OPENMP
#pragma omp parallel
{
    #pragma omp sections
    {
        #pragma omp section
        {
            CalcViewMatrix();
        }
        #pragma omp section
        {
            CalcProjectionMatrix();
        }
    }
}
#else
    CalcViewMatrix();
    CalcProjectionMatrix();
#endif // MR_USE_OPENMP
    CalcMVP();
}

void MR::Camera::SetModelMatrix(glm::mat4* m) {
    _modelMatrix = m;
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

MR::Camera::Camera(const glm::vec3& camPos, const glm::vec3& camTarget, const float& fov, const float& nearZ, const float& farZ, const float& aspectR)
    : _cmode(CameraMode::CM_TARGET), _cproj(CameraProjection::CP_PERSPECTIVE), _pos(camPos), _direction( new glm::vec3(MR::Transform::WorldForwardVector()) ), _left_direction( new glm::vec3(MR::Transform::WorldLeftVector()) ), _target(camTarget), _up(glm::vec3(0,1,0)),
      _fovY(fov), _zNear(nearZ), _zFar(farZ), _aspectRatio(aspectR), _ortho_left(0.0f), _ortho_right(100.0f), _ortho_top(0.0f), _ortho_bottom(100.0f), _modelMatrix(nullptr), _autoReCalc(false), _render_target(nullptr) {

    //allocate space, functionality like calc()
    _viewMatrix = new glm::mat4(glm::lookAt(_pos, _target, _up));
    _projectionMatrix = new glm::mat4(glm::perspective(_fovY, _aspectRatio, _zNear, _zFar));
    _mvp = new glm::mat4( (*_projectionMatrix) * (*_viewMatrix) );
}

MR::Camera::~Camera() {
    delete _direction;
    delete _left_direction;
    delete _viewMatrix;
    delete _projectionMatrix;
    delete _modelMatrix;
    delete _mvp;
}

MR::Camera::Camera(const MR::Camera&)
    : _cmode(CameraMode::CM_TARGET), _cproj(CameraProjection::CP_PERSPECTIVE), _pos(), _direction( 0 ), _left_direction( 0 ), _target(), _up(glm::vec3(0,1,0)),
      _fovY(90.0f), _zNear(0.0f), _zFar(100.0f), _aspectRatio(1.0f), _ortho_left(0.0f), _ortho_right(100.0f), _ortho_top(0.0f), _ortho_bottom(100.0f), _modelMatrix(nullptr), _autoReCalc(false), _render_target(nullptr) {
}

MR::Camera MR::Camera::operator=(const MR::Camera& c){
    return c;
}
