#pragma once

#include "Node.hpp"

namespace mr {

class IModel;
typedef std::weak_ptr<IModel> ModelWeakPtr;

class Entity : public SceneNode {
public:
    virtual void Draw();

    Entity(ModelWeakPtr modelPtr);
    virtual ~Entity();
protected:
    Entity();
    ModelWeakPtr _model;
};

typedef std::shared_ptr<Entity> EntityPtr;

}
