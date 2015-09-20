#pragma once

#ifndef _MR_SCENE_INTERFACES_H_
#define _MR_SCENE_INTERFACES_H_

#include "../Types.hpp"
#include "Transformation.hpp"

#include "../pre_glm.hpp"

namespace mr {
/*
class IEntity : public Copyable<IEntity*> {
public:
    virtual std::string GetName() = 0;
    virtual MR::Transform* GetTransformPtr() = 0;
    virtual MR::BoundingBox* GetBBPtr() = 0;
};
*/

class ShaderManager;

class ICamera {
public:
    mu::Event<ICamera*, const glm::vec3&> OnPositionChanged;
    mu::Event<ICamera*, const glm::vec3&> OnRotationChanged;
    //mu::Event<ICamera*, const glm::vec3&> OnTargetChanged;
    mu::Event<ICamera*> OnDirectionsChanged;

    mu::Event<ICamera*, glm::mat4*> OnMatUpdated_View;
    mu::Event<ICamera*, glm::mat4*> OnMatUpdated_Projection;

    virtual void SetAutoRecalc(const bool& state) = 0;
    virtual bool GetAutoRecalc() = 0;

    virtual glm::vec3 GetForward() = 0;
    virtual glm::vec3 GetLeft() = 0;
    virtual glm::vec3 GetUp() = 0;

    virtual glm::vec3 GetPosition() = 0;
    virtual glm::vec3 GetRotation() = 0;
    virtual glm::vec3 GetTarget() = 0;

    virtual glm::vec3* GetPositionPtr() = 0;

    virtual float GetFovY() = 0;
    virtual float GetNearZ() = 0;
    virtual float GetFarZ() = 0;
    virtual float GetAspectRatio() = 0;

    virtual void SetFovY(const float& f) = 0;
    virtual void SetNearZ(const float& f) = 0;
    virtual void SetFarZ(const float& f) = 0;
    virtual void SetAspectRatio(const float& f) = 0;

    virtual glm::mat4* GetViewMatrixPtr() = 0;
    virtual glm::mat4* GetProjectMatrixPtr() = 0;

    virtual void MoveForward(const float& v) = 0;
    virtual void MoveLeft(const float& v) = 0;
    virtual void MoveUp(const float& v) = 0;

    virtual void Move(const glm::vec3& v) = 0; //translate position

    virtual void Roll(const float& v) = 0; //move x
    virtual void Yaw(const float& v) = 0; //move y
    virtual void Pitch(const float& v) = 0; //move z

    virtual void SetPosition(const glm::vec3& p) = 0;
    virtual void SetRotation(const glm::vec3& p) = 0;

    virtual void CalcViewMatrix() = 0;
    virtual void CalcProjectionMatrix() = 0;
    virtual void Calc() = 0;

    virtual bool SetUniformsRef(ShaderManager* shaderManager) = 0;

    virtual ~ICamera() {}
};

}

#endif // _MR_SCENE_INTERFACES_H_
