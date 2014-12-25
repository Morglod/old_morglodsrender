#pragma once

#include "SceneManagerInterfaces.hpp"

namespace mr {

class Entity;
typedef std::shared_ptr<Entity> EntityPtr;

class SceneNode;
typedef std::shared_ptr<SceneNode> SceneNodePtr;

class IModel;
typedef std::shared_ptr<IModel> ModelPtr;

class SceneManager : public ISceneManager {
public:
    size_t PickLod(float distance, IModel*) const override;
    SceneNode* GetRootNode() const override { return _rootNode.get(); }
    SceneNodePtr GetRootNodeHandle() const override { return _rootNode; }
    EntityPtr GetEntity(size_t const& index) const override { return _entities[index]; }

    EntityPtr CreateEntity(ModelPtr model) override;

    SceneManager();
    virtual ~SceneManager();
protected:
    mr::TDynamicArray<EntityPtr> _entities;
    SceneNodePtr _rootNode;
};

}
