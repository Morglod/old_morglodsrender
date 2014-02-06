#pragma once

#ifndef _MR_SCENE_H_
#define _MR_SCENE_H_

#include "pre.hpp"
#include "Events.hpp"
#include "Entity.hpp"

namespace MR{

class Entity;
class Camera;
class RenderContext;

class SceneManager {
public:
    /** CAMERAS **/
    MR::Event<Camera*> OnMainCameraChanged;
    MR::Event<Camera*> OnCameraCreated;
    MR::Event<Camera*> OnCameraDeletting; //before deleted
    MR::Event<Camera*> OnCameraAdded;
    MR::Event<Camera*> OnCameraRemoving; //before removed

    virtual void AddCamera(Camera* cam);
    virtual void RemoveCamera(Camera* cam);
    virtual Camera* CreateCamera(const glm::vec3& pos, const glm::vec3& direction, const float& fovY, const float& nearClipPlane, const float& farClipPlane, const float& aspectR);
    virtual void DeleteCamera(Camera* cam); //Camera should be in cameras list
    virtual void SetMainCamera(Camera* cam); //Camera should be in cameras list

    /** ENTITIES **/
    MR::Event<Entity*> OnEntityAdded;
    MR::Event<Entity*> OnEntityRemoving; //before removed
    MR::Event<Entity*> OnEntityCreated;
    MR::Event<Entity*> OnEntityDeletting; //before deleted

    virtual void AddEntity(Entity* ent);
    virtual void RemoveEntity(Entity* ent);
    virtual Entity* CreateEntity(MR::Model* model);
    virtual Entity* CreateEntity(const std::string& modelSrc);
    virtual void DeleteEntity(Entity* ent);

    /** **/
    virtual void Draw(RenderContext& rc);
    virtual void Draw(RenderContext* rc);

    SceneManager();
    virtual ~SceneManager();

protected:
    std::vector<Entity*> _entities;
    std::vector<Camera*> _cameras;
    Camera* _mainCamera;
};

}

#endif // _MR_SCENE_H_
