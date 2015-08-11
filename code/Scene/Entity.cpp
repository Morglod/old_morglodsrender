#include "Entity.hpp"
#include "../Models/ModelInterfaces.hpp"

namespace mr {

void Entity::Draw() {
    if(! _model.expired()) {
        _model.lock()->GetLod(0)->Draw(&_mat);
    }
}

Entity::Entity() : _model(), _name("noname") {  }
Entity::Entity(ModelWeakPtr modelPtr, std::string const& name) : _model(modelPtr), _name(name) {  }
Entity::~Entity() {  }

}
