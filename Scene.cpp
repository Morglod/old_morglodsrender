#include "Scene.hpp"
#include "Camera.hpp"
#include "RenderSystem.hpp"
#include "Model.hpp"
#include "Utils/Log.hpp"

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

MR::Entity** MR::SceneManager::CullEntities(Camera* cam){
    //std::vector<MR::Entity*> ent;

    // MAKE CAMERA AABB
    //float camH = glm::sin(cam->GetFovY());

    // CULL ENTITIES
    /*for(auto it = _entities.begin(); it != _entities.end(); ++it){
        if((*it)->GetBBP()->TestAABB()) ent.push_back(*it);
    }*/
    return _entities.data();
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
    Entity* ent = CreateEntity(model);
    AddEntity(ent);
    return ent;
}

Entity* MR::SceneManager::CreateEntity(MR::Model* model) {
    Entity* ent = MR::Entity::CreateEntity(model);
    AddEntity(ent);
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

void MR::SceneManager::AddLight(ILightSource* ls){
    _lights.push_back(ls);
}

void MR::SceneManager::RemoveLight(ILightSource* ls){
    std::vector<ILightSource*>::iterator it = std::find(_lights.begin(), _lights.end(), ls);
    if(it == _lights.end()) return;
    _lights.erase(it);
}

LightsList MR::SceneManager::MakeLightsList(Entity* ent){
    return LightsList::MakeList(ent->GetBBP(), _lights.data(), _lights.size());
}

void MR::SceneManager::SetFog(const float& minDist, const float& maxDist, const glm::vec4& color){
    _fogMin = minDist;
    _fogMax = maxDist;
    _fogColor = color;
}

void MR::SceneManager::GetFogInfo(float* minDist, float* maxDist, glm::vec4* color){
    if(minDist) *minDist = _fogMin;
    if(maxDist) *maxDist = _fogMax;
    if(color) *color = _fogColor;
}

void MR::SceneManager::Draw(IRenderSystem* rc){
    EntityDrawParams edparams;
    edparams._fogColor = &_fogColor;
    edparams._fogMax = &_fogMax;
    edparams._fogMin = &_fogMin;

    for(size_t i = 0; i < _entities.size(); ++i){
        LightsList ll = MakeLightsList(_entities[i]);
        edparams._llist = &ll;
        rc->DrawEntity(_entities[i], &edparams);
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
