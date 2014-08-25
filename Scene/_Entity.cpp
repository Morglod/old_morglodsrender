#include "Entity.hpp"
#include "../Model.hpp"

namespace MR {

void Entity::SetModel(MR::Model* m) {
    if(_model!=m) {
        _model = m;
        CalcBoundingBox();
        OnModelChanged(this, m);
    }
}

void Entity::SetMaterial(MR::Material* m) {
    if(_material != m) {
        _material = m;
        OnMaterialChanged(this, m);
    }
}

void Entity::CalcBoundingBox(){
    if(_model) _bb.ReMake(_model->GetAABBP()->GetMin(), _model->GetAABBP()->GetMax(), _tranform.GetMat());
    else _bb.ReMake(glm::vec3(0,0,0), glm::vec3(0,0,0), _tranform.GetMat());
}

Entity::Entity() : _model(nullptr), _material(nullptr), _bb(glm::vec3(0,0,0), 1, glm::mat4(1.0f)) {
    _tce = new TransformChangedEvent(this);
}

Entity::Entity(MR::Model* m) : _model(m), _material(nullptr), _bb(glm::vec3(0,0,0), 1, glm::mat4(1.0f)) {
    _tce = new TransformChangedEvent(this);
    CalcBoundingBox();
}

Entity::~Entity() {
    delete _tce;
}

Entity* Entity::CreateEntity(MR::Model* model) {
    return new Entity(model);
}

IEntity* Entity::Copy() {
    Entity* nent = new Entity(GetModel());
    nent->_tranform = _tranform;
    return dynamic_cast<MR::IEntity*>(nent);
}
/*
InstancedEntity::InstancedEntity(MR::Model* m, Transform** tr, unsigned int inum, StoragePolicy sp = StoragePolicy::SP_STATIC) :
    Entity(m), spolicy(sp), tBuffer(0), instancesNum(inum)
    {
        glm::mat4* tr_buffer = new glm::mat4[inum]; //model matrixies
        for(unsigned int it = 0; it < inum; ++it){
            tr_buffer[inum] = tr[inum]->mat;
        }

        glGenBuffers(1, &tBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, tBuffer);
        if(sp == StoragePolicy::SP_DYNAMIC) glBufferData(GL_ARRAY_BUFFER, inum * sizeof(glm::mat4), tr_buffer, GL_STREAM_DRAW);
        else glBufferData(GL_ARRAY_BUFFER, inum * sizeof(glm::mat4), tr_buffer, GL_STATIC_DRAW);
    }
*/
}

