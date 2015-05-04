#pragma once

#include "SceneManagerInterfaces.hpp"

namespace mr {

class IVertexFormat;

class SceneManager : public ISceneManager {
public:
    size_t PickLod(float distance, IModel*) const override;
    SceneNode* GetRootNode() const override { return _rootNode.get(); }
    SceneNodePtr GetRootNodeHandle() const override { return _rootNode; }
    EntityPtr GetEntity(size_t const& index) const override { return _entities[index]; }

    EntityPtr CreateEntity(ModelPtr model, std::string const& name = "noname") override;
    EntityPtr CreateEntity(ModelPtr model, SceneNodePtr parentSceneNode, std::string const& name = "noname") override;

    size_t GetEntityNum() const override { return _entities.GetNum(); }
    EntityPtr FindEntity(std::string const& name) const override;
    mr::TStaticArray<EntityPtr> FindEntities(ModelWeakPtr model) const override;

    void Optimize() override;
    void Draw() const override;

    SceneManager();
    virtual ~SceneManager();
protected:
    void _OnSceneChanged() override;

    mr::TDynamicArray<EntityPtr> _entities;
    //std::unordered_map<mr::IVertexFormat*, mr::TDynamicArray<Entity*>> _sort_entity;
    SceneNodePtr _rootNode;
};

}
