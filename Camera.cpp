#include "Camera.hpp"
#include "RenderTarget.hpp"

void MR::Camera::Use(const GLuint& matrixUniform) {
    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &(*mvp)[0][0]);
}

void MR::Camera::MoveForward(const glm::vec3& v) {
    if(v == glm::vec3(0,0,0)) return;
    pos += v * (*direction);
    if(IsAutoRecalc()) CalcViewMatrix();
    OnPositionChanged(this, pos);
}

void MR::Camera::MoveLeft(const glm::vec3& v) {
    if(v == glm::vec3(0,0,0)) return;
    pos += v * (*left_direction);
    if(IsAutoRecalc()) CalcViewMatrix();
    OnPositionChanged(this, pos);
}

void MR::Camera::Move(const glm::vec3& v) {
    if(v == glm::vec3(0,0,0)) return;
    pos += v;
    if(IsAutoRecalc()) CalcViewMatrix();
    OnPositionChanged(this, pos);
}

void MR::Camera::MoveTarget(const glm::vec3& t) {
    if( (cmode == CameraMode::CM_TARGET) && (t != glm::vec3(0,0,0)) ) {
        target += t;
        if(IsAutoRecalc()) CalcViewMatrix();
        OnTargetPosChanged(this, target);
    }
}

void MR::Camera::SetDirection(const glm::vec3& d) {
    if( (*direction) != d ) {
        *direction = d;
        //*left_direction = left_d;
        if(IsAutoRecalc()) CalcViewMatrix();
        OnDirectionChanged(this, d);
    }
}

void MR::Camera::SetDirection(const glm::vec3& d, const glm::vec3 left_d) {
    if( (*direction) != d ) {
        *direction = d;
        *left_direction = left_d;
        if(IsAutoRecalc()) CalcViewMatrix();
        OnDirectionChanged(this, d);
    }
}

void MR::Camera::SetCameraMode(const CameraMode& cm) {
    if(cmode != cm) {
        cmode = cm;
        OnModeChanged(this, cm);
    }
}

void MR::Camera::SetCameraProjection(const CameraProjection& cp) {
    if(cproj != cp) {
        cproj = cp;
        OnProjectionChanged(this, cp);
    }
}

void MR::Camera::CalcViewMatrix() {
    if(cmode == CameraMode::CM_TARGET) *viewMatrix = glm::lookAt(pos, target, up);
    if(cmode == CameraMode::CM_DIRECTION) *viewMatrix = glm::lookAt(pos, pos + (*direction), up);
}

void MR::Camera::CalcProjectionMatrix() {
    if(cproj == CameraProjection::CP_PERSPECTIVE) *projectionMatrix = glm::perspective(fovY, aspectRatio, zNear, zFar);
    if(cproj == CameraProjection::CP_ORTHO) *projectionMatrix = glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, zNear, zFar);
}

void MR::Camera::CalcMVP() {
    if(modelMatrix) *mvp = (*projectionMatrix)*(*viewMatrix)*(*modelMatrix);
    else *mvp = (*projectionMatrix)*(*viewMatrix);
    OnMVPRecalc(this, mvp);
}

void MR::Camera::Calc() {
    CalcViewMatrix();
    //CalcModelMatrix();
    CalcProjectionMatrix();
    CalcMVP();
}

void MR::Camera::SetModelMatrix(glm::mat4* m) {
    modelMatrix = m;
    if(autoReCalc) {
        CalcMVP();
    }
}

void MR::Camera::SetAutoRecalc(const bool& state) {
    autoReCalc = state;
    if(state) Calc();
}

void MR::Camera::SetRenderTarget(MR::RenderTarget* renderTarget){
    if(_render_target != renderTarget){
        _render_target = renderTarget;
        OnRenderTargetChanged(this, renderTarget);
    }
}

MR::Camera::Camera(glm::vec3 camPos, glm::vec3 camTarget, float fov, float nearZ, float farZ, float aspectR)
    : cmode(CameraMode::CM_TARGET), cproj(CameraProjection::CP_PERSPECTIVE), pos(camPos), direction( new glm::vec3(MR::Transform::WorldForwardVector()) ), left_direction( new glm::vec3(MR::Transform::WorldLeftVector()) ), target(camTarget), up(glm::vec3(0,1,0)),
      fovY(fov), zNear(nearZ), zFar(farZ), aspectRatio(aspectR), ortho_left(0.0f), ortho_right(100.0f), ortho_top(0.0f), ortho_bottom(100.0f), modelMatrix(nullptr), autoReCalc(false), _render_target(nullptr) {

    //allocate space, functionality like calc()
    viewMatrix = new glm::mat4(glm::lookAt(pos, target, up));
    projectionMatrix = new glm::mat4(glm::perspective(fovY, aspectRatio, zNear, zFar));
    mvp = new glm::mat4( (*projectionMatrix) * (*viewMatrix) );
}

MR::Camera::~Camera() {
    delete direction;
    delete left_direction;
    delete viewMatrix;
    delete projectionMatrix;
    delete modelMatrix;
    delete mvp;
}
