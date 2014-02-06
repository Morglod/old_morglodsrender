#include "Scene.hpp"
#include "Camera.hpp"
#include "RenderContext.hpp"
#include "Model.hpp"

namespace MR {

void MR::SceneManager::AddCamera(Camera* cam) {
    _cameras.push_back(cam);
    OnCameraAdded(this, cam);
}

void MR::SceneManager::RemoveCamera(Camera* cam) {
    std::vector<Camera*>::iterator it = std::find(_cameras.begin(), _cameras.end(), cam);
    if(it == _cameras.end()) return;
    _cameras.erase(it);
    OnCameraRemoving(this, cam);
}

Camera* MR::SceneManager::CreateCamera(const glm::vec3& pos, const glm::vec3& direction, const float& fovY, const float& nearClipPlane, const float& farClipPlane, const float& aspectR) {
    Camera* cam = new Camera(pos, pos+direction, fovY, nearClipPlane, farClipPlane, aspectR);
    _cameras.push_back(cam);
    OnCameraCreated(this, cam);
    return cam;
}

//Camera should be in cameras list
void MR::SceneManager::DeleteCamera(Camera* cam) {
    std::vector<Camera*>::iterator it = std::find(_cameras.begin(), _cameras.end(), cam);
    if(it == _cameras.end()) return;
    _cameras.erase(it);
    OnCameraDeletting(this, cam);
    OnCameraRemoving(this, cam);
    delete cam;
}

//Camera should be in cameras list
void MR::SceneManager::SetMainCamera(Camera* cam) {
    if(_mainCamera!=cam) {
        _mainCamera = cam;
        OnMainCameraChanged(this, cam);
    }
}

void MR::SceneManager::AddEntity(Entity* ent) {
    _entities.push_back(ent);
    OnEntityAdded(this, ent);
}

void MR::SceneManager::RemoveEntity(Entity* ent) {
    std::vector<Entity*>::iterator it = std::find(_entities.begin(), _entities.end(), ent);
    if(it == _entities.end()) return;
    _entities.erase(it);
    OnEntityRemoving(this, ent);
}

Entity* MR::SceneManager::CreateEntity(const std::string& modelSrc) {
    MR::Model* model = nullptr;
    if(modelSrc != ""){
        model = MR::ModelManager::Instance()->NeedModel(modelSrc);
    }
    return CreateEntity(model);
}

Entity* MR::SceneManager::CreateEntity(MR::Model* model) {
    Entity* ent = MR::Entity::CreateEntity(model);
    OnEntityCreated(this, ent);
    return ent;
}

void MR::SceneManager::DeleteEntity(Entity* ent) {
    std::vector<Entity*>::iterator it = std::find(_entities.begin(), _entities.end(), ent);
    if(it == _entities.end()) return;
    _entities.erase(it);
    OnEntityDeletting(this, ent);
    OnEntityRemoving(this, ent);
    delete ent;
}

void MR::SceneManager::Draw(RenderContext& rc){
    for(MR::Entity* ent : _entities){
        rc.DrawEntity(ent);
    }
}

void MR::SceneManager::Draw(RenderContext* rc){
    for(MR::Entity* ent : _entities){
        rc->DrawEntity(ent);
    }
}

MR::SceneManager::SceneManager() : _mainCamera(nullptr){

}

MR::SceneManager::~SceneManager(){
    for(MR::Entity* ent : _entities){
        delete ent;
    }
    _entities.clear();
}

}
