#include "Entity.hpp"

namespace MR {

void Entity::SetModel(MR::Model* m) {
    if(_model!=m) {
        _model = m;
        OnModelChanged(this, m);
    }
}

void Entity::SetMaterial(MR::Material* m) {
    if(_material != m) {
        _material = m;
        OnMaterialChanged(this, m);
    }
}

Entity::Entity() : _model(nullptr), _material(nullptr) {
}

Entity::Entity(MR::Model* m) : _model(m), _material(nullptr) {
}

Entity::~Entity() {
}

Entity* Entity::CreateEntity(MR::Model* model) {
    return new Entity(model);
}

Entity* Entity::Copy() {
    Entity* nent = new Entity(GetModel());
    nent->_tranform = _tranform;
    return nent;
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

