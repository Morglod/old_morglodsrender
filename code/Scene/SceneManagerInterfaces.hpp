#pragma once

#include "../Models/ModelInterfaces.hpp"

namespace mr {

class SceneNode;
class Entity;
class IModel;
class ICamera;

typedef std::shared_ptr<Entity> EntityPtr;
typedef std::shared_ptr<IModel> ModelPtr;
typedef std::shared_ptr<SceneNode> SceneNodePtr;
typedef std::weak_ptr<IModel> ModelWeakPtr;

struct PointLightDesc {
    glm::vec3 pos, color;
    float innerR, outerR;
    inline PointLightDesc(glm::vec3 const& p, glm::vec3 const& c, float iR, float oR) : pos(p), color(c), innerR(iR), outerR(oR) {}
};

struct PointLightDescList {
    int num = 0;
    std::vector<PointLightDesc> pointLights;

    inline PointLightDesc& Create(glm::vec3 const& pos, glm::vec3 const& color, float innerR, float outerR) {
        pointLights.push_back(PointLightDesc(pos, color, innerR, outerR));
        num++;
        return pointLights[pointLights.size()-1];
    }
};

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
    virtual void Optimize() = 0;
    virtual void SetMainCamera(ICamera* camera) = 0;

    virtual PointLightDesc& CreatePointLight(glm::vec3 const& pos, glm::vec3 const& color, float innerR, float outerR) = 0;
    //TODO: virtual void SetLight(index, PointLightDesc)
    //TODO: virtual void TouchLight(index, PointLightDesc); update light info in gpu buffer
    virtual bool CompleteLights() = 0;

    virtual ~ISceneManager() {}
protected:
    virtual void _OnSceneChanged() = 0;
};

}
