#pragma once

#include "build.hpp"
#include "pre/glm.hpp"

namespace mr {

class MR_API PerspectiveCamera final {
public:
    inline void SetAutoRecalc(bool state);
    void CalcDir();
    void CalcMat();
    inline void Update();
    inline glm::mat4 GetMat() const;
    inline void SetPos(glm::vec3 const& pos);
    inline void SetRot(glm::vec3 const& rot);
    inline glm::vec3 GetForward() const;
    inline glm::vec3 GetBackward() const;
    inline glm::vec3 GetPos() const;
    inline glm::vec3 GetRot() const;
    inline glm::vec3 GetLeft() const;
    inline glm::vec3 GetRight() const;
    inline glm::vec3 GetUp() const;
    inline glm::vec3 GetDown() const;
    inline void MoveForward(float units);
    inline void RotateY(float units);
    inline void RotateX(float units);
private:
    glm::vec3 _pos, _rot;
    glm::vec3 _dir_forward, _dir_left, _dir_up;
    glm::mat4 _mat;
    bool _auto_recalc = true;
    bool _lock_x = false;
    bool _lock_up = true;
};

inline void PerspectiveCamera::SetAutoRecalc(bool state) {
    if(_auto_recalc == state) return;
    if(state) Update();
    _auto_recalc = state;
}

inline void PerspectiveCamera::Update() {
    CalcDir();
    CalcMat();
}

inline glm::mat4 PerspectiveCamera::GetMat() const {
    return _mat;
}

inline void PerspectiveCamera::SetPos(glm::vec3 const& pos) {
    _pos = pos;
    if(_auto_recalc) Update();
}

inline void PerspectiveCamera::SetRot(glm::vec3 const& rot) {
    _rot = rot;
    if(_auto_recalc) Update();
}

inline glm::vec3 PerspectiveCamera::GetForward() const {
    return _dir_forward;
}

inline glm::vec3 PerspectiveCamera::GetBackward() const {
    return -_dir_forward;
}

inline glm::vec3 PerspectiveCamera::GetPos() const {
    return _pos;
}

inline glm::vec3 PerspectiveCamera::GetRot() const {
    return _rot;
}

inline glm::vec3 PerspectiveCamera::GetLeft() const {
    return _dir_left;
}

inline glm::vec3 PerspectiveCamera::GetRight() const {
    return -_dir_left;
}

inline glm::vec3 PerspectiveCamera::GetUp() const {
    return _dir_up;
}

inline glm::vec3 PerspectiveCamera::GetDown() const {
    return -_dir_up;
}

inline void PerspectiveCamera::MoveForward(float units) {
    _pos += _dir_forward * units;
    if(_auto_recalc) Update();
}

inline void PerspectiveCamera::RotateY(float units) {
    _rot.x += units;
    if(_auto_recalc) Update();
}

inline void PerspectiveCamera::RotateX(float units) {
    _rot.y += units;
    if(_auto_recalc) Update();
}

}
