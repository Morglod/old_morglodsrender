#pragma once

#include "../Models/ModelInterfaces.hpp"

namespace mr {

class SceneNode;
class Entity;
class IModel;

typedef std::shared_ptr<Entity> EntityPtr;
typedef std::shared_ptr<IModel> ModelPtr;

class ISceneManager {
public:
    virtual size_t PickLod(float distance, IModel*) const = 0;
    virtual SceneNode* GetRootNode() const = 0;
    virtual SceneNodePtr GetRootNodeHandle() const = 0;
    virtual EntityPtr GetEntity(size_t const& index) const = 0;
    virtual EntityPtr CreateEntity(ModelPtr model) = 0;

    virtual ~ISceneManager() {}
};

}
