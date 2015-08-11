#include "PerspectiveCamera.hpp"
#include "../Shaders/ShaderManager.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "../Utils/Log.hpp"

//#define MR_DEBUG_LOG_CAMERA

#ifdef MR_DEBUG_LOG_CAMERA
#define __MR_DEBUG_LOG(x) mr::Log::LogString(std::string("In PerspectiveCamera ") + x);
#else
#define __MR_DEBUG_LOG(x)
#endif

#define MR_DEBUG_LOG_CAM(x) __MR_DEBUG_LOG(x);
#define glmV3_ToString(_x_) ("(" + std::to_string(_x_.x) + ", " + std::to_string(_x_.y) + ", " + std::to_string(_x_.z) + ")")

namespace mr {

void PerspectiveCamera::MoveForward(const float& v) {
    if(v == 0.0f) return;
    _pos += GetForward() * v;

    if(GetAutoRecalc()) Calc();

    OnPositionChanged(this, _pos);

    MR_DEBUG_LOG_CAM("MoveForward by " + std::to_string(v))
}

void PerspectiveCamera::MoveLeft(const float& v)  {
    if(v == 0.0f) return;
    _pos += GetLeft() * v;

    if(GetAutoRecalc()) Calc();

    OnPositionChanged(this, _pos);

    MR_DEBUG_LOG_CAM("MoveLeft by " + std::to_string(v))
}

void PerspectiveCamera::MoveUp(const float& v)  {
    if(v == 0.0f) return;
    _pos += GetUp() * v;

    if(GetAutoRecalc()) Calc();

    OnPositionChanged(this, _pos);

    MR_DEBUG_LOG_CAM("MoveUp by " + std::to_string(v))
}

void PerspectiveCamera::Move(const glm::vec3& v)  {
    if(v == glm::vec3(0.0f,0.0f,0.0f)) return;
    SetPosition(GetPosition()+v);

    MR_DEBUG_LOG_CAM("Move by " + glmV3_ToString(v))
}

void PerspectiveCamera::Roll(const float& v)  {
    if(v == 0) return;
    _rot.x += v;
    _CalcDirectionsFromRot();

    if(GetAutoRecalc()) Calc();

    MR_DEBUG_LOG_CAM("Roll for " + std::to_string(v))
}

void PerspectiveCamera::Yaw(const float& v)  {
    if(v == 0) return;
    _rot.y += v;
    _CalcDirectionsFromRot();

    if(GetAutoRecalc()) Calc();

    MR_DEBUG_LOG_CAM("Yaw for " + std::to_string(v))
}

void PerspectiveCamera::Pitch(const float& v)  {
    if(v == 0) return;
    _rot.z += v;
    _CalcDirectionsFromRot();

    if(GetAutoRecalc()) Calc();

    MR_DEBUG_LOG_CAM("Pitch for " + std::to_string(v))
}

void PerspectiveCamera::SetPosition(const glm::vec3& p)  {
    _pos = p;

    if(GetAutoRecalc()) Calc();
    OnPositionChanged(this, p);

    MR_DEBUG_LOG_CAM("New position" + glmV3_ToString(_pos))
}

void PerspectiveCamera::SetRotation(const glm::vec3& p)  {
    _rot = p;
    _CalcDirectionsFromRot();

    if(GetAutoRecalc()){
        Calc();
    }

    OnRotationChanged(this, _rot);
    OnDirectionsChanged(this);

    MR_DEBUG_LOG_CAM("New rotation" + glmV3_ToString(_rot))
}

void PerspectiveCamera::CalcViewMatrix()  {
    _mat_view = glm::lookAt(this->GetPosition(), this->GetTarget(), this->GetUp());
    OnMatUpdated_View(this, this->GetViewMatrixPtr());

    MR_DEBUG_LOG_CAM("View matrix calculated")
}

void PerspectiveCamera::CalcProjectionMatrix()  {
    _mat_proj = glm::perspective(this->GetFovY(), this->GetAspectRatio(), this->GetNearZ(), this->GetFarZ());
    OnMatUpdated_Projection(this, this->GetProjectMatrixPtr());

    MR_DEBUG_LOG_CAM("Projection matrix calculated")
}

void PerspectiveCamera::Calc()  {
    CalcViewMatrix();
    CalcProjectionMatrix();
}

void PerspectiveCamera::_CalcDirectionsFromRot() {
    _dir_forward = mr::Transform::NormalizedDirection(_rot.x, _rot.y, _rot.z);
    _dir_up = mr::Transform::NormalizedDirection(_rot.x, _rot.y+90.0f, _rot.z);
    _dir_left = mr::Transform::NormalizedDirection(_rot.x+90.0f, 0.0f, _rot.z);
    OnDirectionsChanged(this);
}

bool PerspectiveCamera::SetUniformsRef(ShaderManager* shaderManager) {
    shaderManager->SetGlobalUniform("MR_MAT_VIEW", mr::IShaderUniformRef::Mat4, GetViewMatrixPtr());
    shaderManager->SetGlobalUniform("MR_MAT_PROJ", mr::IShaderUniformRef::Mat4, GetProjectMatrixPtr());
    shaderManager->SetGlobalUniform("MR_CAM_POS", mr::IShaderUniformRef::Vec3, GetPositionPtr());
    return true;
}

PerspectiveCamera::PerspectiveCamera() {
}

PerspectiveCamera::PerspectiveCamera(const glm::vec3& pos, const glm::vec3& rotation, const float& fovY, const float& aspectR, const float& nearZ, const float& farZ)
 : _auto_recalc(false), _dir_forward(0,0,0), _dir_left(0,0,0), _dir_up(0,0,0), _pos(0,0,0), _rot(0,0,0), _fovy(0.0f), _near_z(0.0f), _far_z(0.0f), _aspectr(0.0f), _mat_view(1.0f), _mat_proj(1.0f)
{
    _pos = pos;
    SetRotation(rotation);
    SetFovY(fovY);
    SetAspectRatio(aspectR);
    SetNearZ(nearZ);
    SetFarZ(farZ);
    Calc();
    _auto_recalc = true;
}

PerspectiveCamera::~PerspectiveCamera() {
}

}
