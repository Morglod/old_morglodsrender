#include "Entity.hpp"

namespace MR {

Entity::Entity(MR::Model* m) : model(m), material(nullptr) {
}

Entity* Entity::CreateEntity(MR::Model* model) {
    return new Entity(model);
}

Entity* Entity::Copy(){
    Entity* nent = new Entity(GetModel());
    nent->tranform = tranform;
    return nent;
}

}
