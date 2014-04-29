#pragma once

#ifndef _MR_SCENE_H_
#define _MR_SCENE_H_

#include "Config.hpp"
#include "Events.hpp"
#include "Entity.hpp"

namespace MR{

class Entity;
class Camera;
class IRenderSystem;
class ILightSource;
class LightsList;

class SceneManager {
public:
    /** CAMERAS **/
    MR::EventListener<SceneManager*, Camera*> OnMainCameraChanged;
    MR::EventListener<SceneManager*, Camera*> OnCameraCreated;
    MR::EventListener<SceneManager*, Camera*> OnCameraDeletting; //before deleted
    MR::EventListener<SceneManager*, Camera*> OnCameraAdded;
    MR::EventListener<SceneManager*, Camera*> OnCameraRemoving; //before removed

    virtual void AddCamera(Camera* cam);
    virtual void RemoveCamera(Camera* cam);
    virtual Camera* CreateCamera(const glm::vec3& pos, const glm::vec3& direction, const float& fovY, const float& nearClipPlane, const float& farClipPlane, const float& aspectR);
    virtual void DeleteCamera(Camera* cam); //Camera should be in cameras list
    virtual void SetMainCamera(Camera* cam); //Camera should be in cameras list

    /** ENTITIES **/
    MR::EventListener<SceneManager*, Entity*> OnEntityAdded;
    MR::EventListener<SceneManager*, Entity*> OnEntityRemoving; //before removed
    MR::EventListener<SceneManager*, Entity*> OnEntityCreated;
    MR::EventListener<SceneManager*, Entity*> OnEntityDeletting; //before deleted

    virtual void AddEntity(Entity* ent);
    virtual void RemoveEntity(Entity* ent);
    virtual Entity* CreateEntity(MR::Model* model);
    virtual Entity* CreateEntity(const std::string& modelSrc);
    virtual void DeleteEntity(Entity* ent);

    /** **/
    virtual void Draw(IRenderSystem* rc);

    SceneManager();
    virtual ~SceneManager();

protected:
    std::vector<Entity*> _entities;
    std::vector<Camera*> _cameras;
    std::vector<ILightSource*> _lights;

    Camera* _mainCamera;

    bool _light; //toggle lights
};

}

#endif // _MR_SCENE_H_
