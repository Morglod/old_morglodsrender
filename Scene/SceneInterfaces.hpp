#pragma once

#ifndef _MR_SCENE_INTERFACES_H_
#define _MR_SCENE_INTERFACES_H_

#include "../Types.hpp"
#include "../Boxes.hpp"
#include "Transformation.hpp"

namespace MR {

class IEntity;

class IEntity : public Copyable<IEntity*> {
public:
    virtual std::string GetName() = 0;
    virtual MR::Transform* GetTransformPtr() = 0;
    virtual MR::BoundingBox* GetBBPtr() = 0;
};

}

#endif // _MR_SCENE_INTERFACES_H_
