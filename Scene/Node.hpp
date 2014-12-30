#pragma once

#include "../Utils/Containers.hpp"

#include <glm/glm.hpp>

namespace mr {

class SceneNode;
typedef std::shared_ptr<SceneNode> SceneNodePtr;
typedef std::weak_ptr<SceneNode> SceneNodeWeakPtr;

class SceneNode {
public:
    virtual void SetParent(SceneNodePtr sceneNode);
    SceneNodeWeakPtr GetParent() const;

    SceneNodePtr GetChild(size_t const& childIndex) const;
    size_t GetChildrenNum() const;

    virtual SceneNodePtr CreateChild();
    virtual SceneNodePtr AddChild(SceneNodePtr newChild);
    virtual SceneNodePtr RemoveChild(SceneNodePtr childToRemove);
    virtual SceneNodePtr RemoveChild(SceneNode* childToRemove);

    //CalcMat & UpdateChildren
    //call it if node is reattached or parent's transformations changed.
    virtual void Update();

    inline glm::mat4 GetMat() const { return _mat; }
    inline glm::mat4 GetLocalMat() const { return _localMat; }

    virtual void SetPos(glm::vec3 const& pos) { _pos = pos; if(_autoUpdate) { CalcLocalMat(); Update(); } }
    virtual void SetRot(glm::vec3 const& rot) { _rot = rot; if(_autoUpdate) { CalcLocalMat(); Update(); } }
    virtual void SetScale(glm::vec3 const& scale) { _scale = scale; if(_autoUpdate) { CalcLocalMat(); Update(); } }

    virtual glm::vec3 GetPos() const { return _pos; }
    virtual glm::vec3 GetRot() const { return _rot; }
    virtual glm::vec3 GetScale() const { return _scale; }

    inline glm::vec3& Pos() { return _pos; }
    inline glm::vec3& Rot() { return _rot; }
    inline glm::vec3& Scale() { return _scale; }

    inline void SetAutoUpdate(bool autoUpdate) { _autoUpdate = autoUpdate; if(autoUpdate) Update(); }
    inline bool GetAutoUpdate() const { return _autoUpdate; }

    SceneNode();
    SceneNode(SceneNodePtr parent);
    SceneNode(SceneNode const& cpy) = delete;
    SceneNode(SceneNode& cpy) = delete;
    virtual ~SceneNode();
protected:
    virtual void CalcLocalMat();
    virtual void CalcMat(glm::mat4 const& parentMat);
    virtual void UpdateChildrenMat();

    SceneNodePtr _parent;
    std::deque<SceneNodePtr> _children;

    glm::mat4 _mat, _localMat;
    glm::vec3 _pos, _rot, _scale;

    bool _autoUpdate;
};

}
