#pragma once

#include "Node.hpp"

namespace mr {

class IModel;
typedef std::weak_ptr<IModel> ModelWeakPtr;

class Entity : public SceneNode {
public:
    inline std::string GetName() const { return _name; }
    inline ModelWeakPtr GetModel() const { return _model; }

    virtual void Draw();

    Entity(ModelWeakPtr modelPtr, std::string const& name = "noname");
    virtual ~Entity();
protected:
    Entity();
    ModelWeakPtr _model;
    std::string _name;
};

typedef std::shared_ptr<Entity> EntityPtr;

}
