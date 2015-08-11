#pragma once

#include "SceneManagerInterfaces.hpp"

#include <vector>

namespace mr {

class IVertexFormat;
class IBuffer;

class SceneManager : public ISceneManager {
public:
    size_t PickLod(float distance, IModel*) const override;
    SceneNode* GetRootNode() const override { return _rootNode.get(); }
    SceneNodePtr GetRootNodeHandle() const override { return _rootNode; }
    EntityPtr GetEntity(size_t const& index) const override { return _entities[index]; }

    EntityPtr CreateEntity(ModelPtr model, std::string const& name = "noname") override;
    EntityPtr CreateEntity(ModelPtr model, SceneNodePtr parentSceneNode, std::string const& name = "noname") override;

    size_t GetEntityNum() const override { return _entities.size(); }
    EntityPtr FindEntity(std::string const& name) const override;
    mr::TStaticArray<EntityPtr> FindEntities(ModelWeakPtr model) const override;

    void Optimize() override;
    void Draw() const override;

    void SetMainCamera(ICamera* camera) override;

    inline PointLightDesc& CreatePointLight(glm::vec3 const& pos, glm::vec3 const& color, float innerR, float outerR) override {
        return _pointLights.Create(pos, color, innerR, outerR);
    }

    bool CompleteLights() override;

    SceneManager();
    virtual ~SceneManager();
protected:
    void _OnSceneChanged() override;

    std::vector<EntityPtr> _entities;
    //std::unordered_map<mr::IVertexFormat*, mr::TDynamicArray<Entity*>> _sort_entity;
    SceneNodePtr _rootNode;
    PointLightDescList _pointLights;
    IBuffer* lightsGpuBuff = nullptr;
};

}
