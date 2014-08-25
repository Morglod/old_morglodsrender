#pragma once

#ifndef _MR_ENTITY_H_
#define _MR_ENTITY_H_

#include "../Config.hpp"
#include "Transformation.hpp"
#include "../Utils/Events.hpp"
#include "../Types.hpp"
//#include "../Boxes.hpp"
#include "SceneInterfaces.hpp"

#include <string>

namespace MR {
class Model;
class Transform;
class Material;

/** Unique for each rendering object
*/
class Entity : public IEntity {
public:
    class TransformChangedEvent : MR::EventHandle<MR::Transform*, const glm::mat4&> {
    public:
        void Invoke(MR::EventListener<Transform*, const glm::mat4&>* eventL, MR::Transform* t, const glm::mat4& mat){
            if(_ent) _ent->CalcBoundingBox();
        }

        TransformChangedEvent(Entity* ent) : _ent(ent) {}
    protected:
        Entity* _ent;
    };

    MR::EventListener<Entity*, MR::Model*> OnModelChanged;
    MR::EventListener<Entity*, MR::Material*> OnMaterialChanged;

    inline MR::Model* GetModel() { return _model; }
    inline MR::Material* GetMaterial(){ return _material; }
    inline std::string GetName() override { return _name; }
    inline MR::Transform* GetTransformPtr() override { return &_tranform; }

    inline MR::BoundingBox* GetBBPtr() override { return &_bb; }
    virtual void CalcBoundingBox();

    virtual void SetModel(MR::Model* m);
    virtual void SetMaterial(MR::Material* m);

    IEntity* Copy() override;

    Entity();
    Entity(MR::Model* m);
    virtual ~Entity();

    static Entity* CreateEntity(MR::Model* model);

protected:
    MR::Model* _model;
    MR::Material* _material; //if not nullptr, used as model's material
    MR::Transform _tranform;
    std::string _name;
    MR::BoundingBox _bb;
    TransformChangedEvent* _tce;
};

}

#endif
