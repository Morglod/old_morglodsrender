#include "Node.hpp"

#include <algorithm>
#include <glm/gtx/transform.hpp>

namespace mr {

void SceneNode::SetParent(SceneNodePtr sceneNode) {
    if(_parent != nullptr) _parent->RemoveChild(this);
    _parent = sceneNode;
    Update();
}

SceneNodeWeakPtr SceneNode::GetParent() const {
    return (_parent != nullptr) ? SceneNodeWeakPtr(_parent) : SceneNodeWeakPtr();
}

SceneNodePtr SceneNode::GetChild(size_t const& childIndex) const {
    return _children[childIndex];
}

size_t SceneNode::GetChildrenNum() const {
    return _children.size();
}

SceneNodePtr SceneNode::CreateChild() {
    return AddChild(SceneNodePtr(new SceneNode()));
}

SceneNodePtr SceneNode::AddChild(SceneNodePtr newChild) {
    _children.push_back(newChild);
    newChild->Update();
    return newChild;
}

SceneNodePtr SceneNode::RemoveChild(SceneNodePtr childToRemove) {
    _children.erase(std::find(_children.begin(), _children.end(), childToRemove));
    childToRemove->Update();
    return childToRemove;
}

SceneNodePtr SceneNode::RemoveChild(SceneNode* childToRemove) {
    SceneNodePtr finded = nullptr;

    std::for_each(_children.begin(), _children.end(),
        [childToRemove, &finded](SceneNodePtr& elementPtr) {
            if((elementPtr).get() == childToRemove) {
                finded = elementPtr;
                return false;
            }
            return true;
        });

    if(finded == nullptr) return nullptr;
    return RemoveChild(finded);
}

void SceneNode::Update() {
    CalcMat( (_parent != nullptr) ? _parent->GetMat() : glm::mat4(1.0f) );
    UpdateChildrenMat();
}

void SceneNode::CalcLocalMat() {
    _localMat = ( glm::translate(glm::mat4(1.0f), _pos) * ( glm::rotate(glm::mat4(1.0f), _rot.x, glm::vec3(1,0,0)) * glm::rotate(glm::mat4(1.0f), _rot.y, glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1.0f), _rot.z, glm::vec3(0,0,1)) ) * glm::scale(glm::mat4(1.0f), _scale) );
}

void SceneNode::CalcMat(glm::mat4 const& parentMat) {
    _mat = _localMat * parentMat;
}

void SceneNode::UpdateChildrenMat() {
    for(size_t i = 0; i < _children.size(); ++i) {
        _children[i]->CalcMat(_mat);
    }
}

SceneNode::SceneNode() : _parent(nullptr), _mat(1.0f), _localMat(1.0f), _pos(0.0f, 0.0f, 0.0f), _rot(0.0f, 0.0f, 0.0f), _scale(1.0f, 1.0f, 1.0f), _autoUpdate(true) {
}

SceneNode::SceneNode(SceneNodePtr parent) : _parent(parent)  {
    Update();
}

SceneNode::~SceneNode() {
}

}
