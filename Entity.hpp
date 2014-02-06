#pragma once

#ifndef _MR_ENTITY_H_
#define _MR_ENTITY_H_

#include "pre.hpp"
#include "Transformation.hpp"
#include "Events.hpp"

namespace MR {
class Model;
class Transform;
class Material;

/** Unique for each rendering object
*/
class Entity : public Copyable<Entity> {
public:
    MR::Event<MR::Model*> OnModelChanged; //new model
    MR::Event<MR::Material*> OnMaterialChanged; //new material

    inline MR::Model* GetModel() { return _model; }
    inline MR::Material* GetMaterial(){ return _material; }
    inline std::string GetName() { return _name; }
    inline MR::Transform* GetTransformP() { return &_tranform; }

    virtual void SetModel(MR::Model* m);
    virtual void SetMaterial(MR::Material* m);

    Entity* Copy();

    Entity();
    Entity(MR::Model* m);
    virtual ~Entity();

    static Entity* CreateEntity(MR::Model* model);

protected:
    MR::Model* _model;
    MR::Material* _material; //if not nullptr, used as model's material
    MR::Transform _tranform;
    std::string _name;
};

}

#endif
