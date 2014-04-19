#pragma once

#ifndef _MR_UI_H_
#define _MR_UI_H_

#include "Events.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

#include <vector>

namespace MR {

class Shader;
class IShaderUniform;

class IRenderSystem;
class IGeometry;
class GeometryBuffer;

class UIElement;
class UIManager;

class IRect {
public:
    virtual glm::vec2 GetPos() = 0;
    virtual glm::vec2 GetSize() = 0;
    virtual void SetPos(const glm::vec2& p) = 0;
    virtual void SetSize(const glm::vec2& s) = 0;
    virtual bool Equal(IRect* rect) = 0;
};

class Rect : public Object, public IRect {
    friend class UIElement;
public:
    inline glm::vec2 GetPos() override;
    inline glm::vec2 GetSize() override;

    inline void SetPos(const glm::vec2& p) override;
    inline void SetSize(const glm::vec2& s) override;

    inline bool Equal(IRect * rect) override {
        if((v2[0] == rect->GetPos()) && (v2[1] == rect->GetSize())) return true;
        return false;
    }

    inline bool operator == (const Rect& r) { return  ( (r.v2[0] == v2[0]) && (r.v2[1] == v2[1]) ); }
    inline bool operator != (const Rect& r) { return !( (r.v2[0] == v2[0]) && (r.v2[1] == v2[1]) ); }

    std::string ToString() override { return "Rect("+std::to_string(v2[0].x)+", "+std::to_string(v2[0].y)+", "+std::to_string(v2[1].x)+", "+std::to_string(v2[1].y)+")"; }

    Rect() : Object(), v2(new glm::vec2[2]) {}
    Rect(const glm::vec2& p, const glm::vec2& s) : Object(), v2(new glm::vec2[2]{p,s}) {}
    Rect(const float& px, const float& py, const float& sx, const float& sy) : Object(), v2(new glm::vec2[2]{glm::vec2(px,py),glm::vec2(sx,sy)}) {}
    virtual ~Rect(){ delete [] v2; }
protected:
    glm::vec2* v2;
};

class IUIElement {
public:
    MR::EventListener<const Rect&> OnRectChanged;
    MR::EventListener<const glm::vec4&> OnColorChanged;
    MR::EventListener<const glm::vec2&> OnScaleChanged;

    virtual void SetRect(const Rect& r) = 0;
    virtual Rect* GetRectPtr() = 0;

    virtual void SetColor(const glm::vec4& rgba) = 0;
    virtual glm::vec4 GetColor() = 0;

    virtual void Add(IUIElement* element) = 0;
    virtual void Remove(IUIElement* element) = 0;

    virtual void Draw(IRenderSystem* rc, const float& delta) = 0;

    //parent_mat, all parent matricies combined
    virtual void Draw(IRenderSystem* rc, const float& delta, IUIElement* parent, const glm::mat4& parent_mat) = 0;

    virtual ~IUIElement(){}
};

class UIElement : public Object, public IUIElement {
    friend class UIManager;
public:
    void SetRect(const Rect& r) override;
    inline Rect* GetRectPtr() override;

    void SetColor(const glm::vec4& rgba) override;
    inline glm::vec4 GetColor() override;

    //Add already exist / remove from list but don't free memory
    void Add(IUIElement* element) override;
    void Remove(IUIElement* element) override;

    //Create / Free memory
    virtual UIElement* Create(MR::UIManager* m, const Rect& r);
    virtual void Delete(UIElement* element);

    void Draw(IRenderSystem* rc, const float& delta) override;
    void Draw(IRenderSystem* rc, const float& delta, IUIElement* parent, const glm::mat4& parent_mat) override;

    UIElement(UIManager* m, const Rect& r);
    virtual ~UIElement();
protected:
    void _CalcMatrix();

    Rect _rect;
    bool _visible;
    glm::vec4 _color;
    glm::mat4* _model_mat;
    glm::mat4* _model_mat_for_children;
    UIManager* _manager;

    std::vector<IUIElement*> _elements;
};

class UIManager {
    friend class UIElement;
public:
    inline void Add(UIElement* l);
    inline size_t GetLayoutsNum();
    inline UIElement* GetLayout(const size_t& i);
    inline UIElement** GetLayouts();

    virtual void Draw(IRenderSystem* rc, const float& delta);

    static inline UIManager* Instance();

    UIManager();
    virtual ~UIManager();
protected:
    std::vector<UIElement*> _layouts;
    GeometryBuffer* _quad_geom_buffer;
    IGeometry* _quad_geom;
    glm::mat4* _modelMatrix;
    glm::vec4* _color;
    int* _tex_unit;
    MR::Shader* _shader;
};

}

glm::vec2 MR::Rect::GetPos(){
    return v2[0];
}

glm::vec2 MR::Rect::GetSize(){
    return v2[1];
}

void MR::Rect::SetPos(const glm::vec2& p){
    v2[0] = p;
}

void MR::Rect::SetSize(const glm::vec2& s){
    v2[1] = s;
}

MR::Rect* MR::UIElement::GetRectPtr() {
    return &_rect;
}

glm::vec4 MR::UIElement::GetColor(){
    return _color;
}

void MR::UIManager::Add(UIElement* l){
    _layouts.push_back(l);
}

size_t MR::UIManager::GetLayoutsNum() {
    return _layouts.size();
}

MR::UIElement* MR::UIManager::GetLayout(const size_t& i) {
    return _layouts[i];
}

MR::UIElement** MR::UIManager::GetLayouts() {
    return _layouts.data();
}

MR::UIManager* MR::UIManager::Instance() {
    static UIManager* m = new UIManager();
    return m;
}

#endif //_MR_UI_H_
