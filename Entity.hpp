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

/** Unique for each world object
*/
class Entity : public Copyable<Entity> {
protected:
    MR::Model* model;
    MR::Material* material; //if not nullptr, used as model's material
    MR::Transform tranform;

public:

    /** sender - Entity
     *  arg1 - new model
     */
    MR::Event<MR::Model*> OnModelChanged;

    /** sender - Entity
     *  arg1 - new material
     */
    MR::Event<MR::Material*> OnMaterialChanged;

    inline MR::Model* GetModel() {
        return model;
    }

    inline void SetModel(MR::Model* m){
        model = m;
        OnModelChanged(this, m);
    }

    inline MR::Transform* GetTransform() {
        return &tranform;
    }

    inline MR::Material* GetMaterial(){
        return material;
    }

    inline void SetMaterial(MR::Material* m){
        material = m;
        OnMaterialChanged(this, m);
    }

    Entity* Copy();

    Entity() : model(nullptr), material(nullptr){}
    Entity(MR::Model* m);
    virtual ~Entity(){}

    static Entity* CreateEntity(MR::Model* model);
};

}

#endif
