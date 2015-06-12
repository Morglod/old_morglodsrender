#include "SceneManager.hpp"
#include "../Buffers/BufferManager.hpp"
#include "../Shaders/ShaderManager.hpp"
#include "../Shaders/ShaderConfig.hpp"
#include "../StateCache.hpp"

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
    _OnSceneChanged();
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

void SceneManager::Optimize() {
    /*_sort_entity.clear();
    for(size_t i = 0; i < _entities.GetNum(); ++i) {
        add to _sort_entity
    }*/
}

void SceneManager::Draw() const {
    //if(_sort_entity.empty()) {
        for(size_t i = 0; i < _entities.GetNum(); ++i) {
            _entities[i]->Draw();
        }
    /*} else {
        for(std::pair<IVertexFormat*, mr::TDynamicArray<Entity*>> const& p : _sort_entity) {
            for(size_t i = 0; i < p.second.GetNum(); ++i) {
                p.second[i]->Draw();
            }
        }
    }*/
}

bool SceneManager::CompleteLights() {
    //TODO test for errors

    if(lightsGpuBuff != nullptr) {
        mr::GPUBufferManager::GetInstance().Delete(lightsGpuBuff);
        lightsGpuBuff = nullptr;
    }

    mr::ShaderManager* shaderManager = mr::ShaderManager::GetInstance();
    mr::ShaderUniformMap* shaderUniformMap = shaderManager->DefaultShaderProgram()->GetMap();
    mr::ShaderUniformBlockInfo* blockInfo = &(shaderUniformMap->GetUniformBlock(MR_SHADER_POINTLIGHTS_BLOCK));

    if((_pointLights.num = _pointLights.pointLights.size()) != 0) {
        lightsGpuBuff = mr::GPUBufferManager::GetInstance().CreateBuffer(mr::IGPUBuffer::FastChange, 16777216); //16mb

        for(int i = 0; i < _pointLights.num; ++i) {
            const std::string uniform_name = "MR_pointLights["+std::to_string(i)+"].";
            lightsGpuBuff->Write(&_pointLights.pointLights[i].pos, 0, blockInfo->GetOffset(uniform_name+"pos"), sizeof(glm::vec3), nullptr, nullptr);
            lightsGpuBuff->Write(&_pointLights.pointLights[i].color, 0, blockInfo->GetOffset(uniform_name+"color"), sizeof(glm::vec3), nullptr, nullptr);
            lightsGpuBuff->Write(&_pointLights.pointLights[i].innerR, 0, blockInfo->GetOffset(uniform_name+"innerRange"), sizeof(float), nullptr, nullptr);
            lightsGpuBuff->Write(&_pointLights.pointLights[i].outerR, 0, blockInfo->GetOffset(uniform_name+"outerRange"), sizeof(float), nullptr, nullptr);
        }
    }

    mr::StateCache::GetDefault()->BindUniformBuffer(lightsGpuBuff, blockInfo->binding);
    shaderManager->SetGlobalUniform("MR_numPointLights", mr::IShaderUniformRef::Int, &_pointLights.num);

    return true;
}

void SceneManager::_OnSceneChanged() {
    //_sort_entity.clear();
}

SceneManager::SceneManager() : _rootNode(new mr::SceneNode()) {
}

SceneManager::~SceneManager() {
}

}
