#pragma once

#include "../Models/ModelInterfaces.hpp"

namespace mr {

class SceneNode;
class Entity;
class IModel;

typedef std::shared_ptr<Entity> EntityPtr;
typedef std::shared_ptr<IModel> ModelPtr;
typedef std::shared_ptr<SceneNode> SceneNodePtr;
typedef std::weak_ptr<IModel> ModelWeakPtr;

class ISceneManager {
public:
    virtual size_t PickLod(float distance, IModel*) const = 0;
    virtual SceneNode* GetRootNode() const = 0;
    virtual SceneNodePtr GetRootNodeHandle() const = 0;
    virtual EntityPtr GetEntity(size_t const& index) const = 0;
    virtual EntityPtr CreateEntity(ModelPtr model, std::string const& name = "noname") = 0;
    virtual EntityPtr CreateEntity(ModelPtr model, SceneNodePtr parentSceneNode, std::string const& name = "noname") = 0;
    virtual size_t GetEntityNum() const = 0;
    virtual EntityPtr FindEntity(std::string const& name) const = 0;
    virtual mr::TStaticArray<EntityPtr> FindEntities(ModelWeakPtr model) const = 0;
    virtual void Draw() const = 0;

    virtual ~ISceneManager() {}
};

}
