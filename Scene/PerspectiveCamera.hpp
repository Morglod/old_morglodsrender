#pragma once

#ifndef _MR_PERSPECTIVE_CAMERA_H_
#define _MR_PERSPECTIVE_CAMERA_H_

#include "SceneInterfaces.hpp"

namespace MR {

class PerspectiveCamera : public ICamera {
public:
    inline void SetAutoRecalc(const bool& state) override { _auto_recalc = state; if(state) Calc(); }
    inline bool GetAutoRecalc() override { return _auto_recalc; }

    inline glm::vec3 GetForwardDirection() override { return _dir_forward; }
    inline glm::vec3 GetLeftDirection() override { return _dir_left; }
    inline glm::vec3 GetUpDirection() override { return _dir_up; }

    inline glm::vec3 GetPosition() override { return _pos; }
    inline glm::vec3 GetRotation() override { return _rot; }
    inline glm::vec3 GetTarget() override { return GetPosition()+GetForwardDirection(); }
    inline glm::vec3 GetUp() override { return _dir_up; }

    inline float GetFovY() override { return _fovy; }
    inline float GetNearZ() override { return _near_z; }
    inline float GetFarZ() override { return _far_z; }
    inline float GetAspectRatio() override { return _aspectr; }

    inline void SetFovY(const float& f) override { _fovy = f; if(this->GetAutoRecalc()) Calc(); }
    inline void SetNearZ(const float& f) override { _near_z = f; if(this->GetAutoRecalc()) Calc(); }
    inline void SetFarZ(const float& f) override { _far_z = f; if(this->GetAutoRecalc()) Calc(); }
    inline void SetAspectRatio(const float& f) override { _aspectr = f; if(this->GetAutoRecalc()) Calc(); }

    inline glm::mat4* GetModelMatrixPtr() override { return _mat_model_ptr; }
    inline glm::mat4* GetViewMatrixPtr() override { return &_mat_view; }
    inline glm::mat4* GetProjectMatrixPtr() override { return &_mat_proj; }
    inline glm::mat4* GetMVPPtr() override { return &_mat_mvp; }

    void MoveForward(const float& v) override;
    void MoveLeft(const float& v) override;
    void MoveUp(const float& v) override;

    void Move(const glm::vec3& v) override;

    void Yaw(const float& v) override;
    void Pitch(const float& v) override;
    void Roll(const float& v) override;

    void SetModelMatrixPtr(glm::mat4* m) override;

    void SetPosition(const glm::vec3& p) override;
    void SetRotation(const glm::vec3& p) override;

    void CalcViewMatrix() override;
    void CalcProjectionMatrix() override;
    void CalcMVP() override;
    void Calc() override;

    PerspectiveCamera();
    PerspectiveCamera(const glm::vec3& pos, const glm::vec3& rotation, const float& fovY, const float& aspectR, const float& nearZ, const float& farZ);
    virtual ~PerspectiveCamera();

protected:
    void _CalcDirectionsFromRot();

    bool _auto_recalc;

    //Calculated
    glm::vec3 _dir_forward, _dir_left, _dir_up;
    //From this
    glm::vec3 _pos, _rot;

    float _fovy, _near_z, _far_z, _aspectr;
    glm::mat4 _mat_view, _mat_proj, _mat_mvp;
    glm::mat4* _mat_model_ptr;

    bool _fixed_target; ///TODO (Third person camera)
};

}

#endif // _MR_PERSPECTIVE_CAMERA_H_
