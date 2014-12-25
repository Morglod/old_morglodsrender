#include "Entity.hpp"
#include "../Models/ModelInterfaces.hpp"

namespace mr {

void Entity::Draw() {
    if(! _model.expired()) {
        _model.lock()->GetLod(0)->Draw(&_mat);
    }
}

Entity::Entity() : _model() {  }
Entity::Entity(ModelWeakPtr modelPtr) : _model(modelPtr) {  }
Entity::~Entity() {  }

}
