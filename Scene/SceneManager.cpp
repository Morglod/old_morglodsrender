#include "SceneManager.hpp"

#include "Node.hpp"
#include "Entity.hpp"

namespace mr {

size_t SceneManager::PickLod(float distance, IModel* model) const {
    return model->GetLodIndexAtDistance(distance);
}

EntityPtr SceneManager::CreateEntity(ModelPtr model, std::string const& name) {
    return CreateEntity(model, GetRootNodeHandle(), name);
}

EntityPtr SceneManager::CreateEntity(ModelPtr model, SceneNodePtr parentSceneNode, std::string const& name) {
    EntityPtr ent = EntityPtr(new Entity(ModelWeakPtr(model), name));
    parentSceneNode->AddChild(std::static_pointer_cast<SceneNode>(ent));
    _entities.PushBack(ent);
    return ent;
}

EntityPtr SceneManager::FindEntity(std::string const& name) const {
    for(size_t i = 0; i < _entities.GetNum(); ++i) {
        if(_entities[i]->GetName() == name) return _entities[i];
    }
    return nullptr;
}

mr::TStaticArray<EntityPtr> SceneManager::FindEntities(ModelWeakPtr model) const {
    mr::TDynamicArray<EntityPtr> found;
    for(size_t i = 0; i < _entities.GetNum(); ++i) {
        if(_entities[i]->GetModel().lock() == model.lock()) found.PushBack( _entities[i] );
    }
    return found.ConvertToStaticArray();
}

void SceneManager::Draw() const {
    for(size_t i = 0; i < _entities.GetNum(); ++i) {
        _entities[i]->Draw();
    }
}

SceneManager::SceneManager() : _rootNode(new mr::SceneNode()) {
}

SceneManager::~SceneManager() {
}

}
