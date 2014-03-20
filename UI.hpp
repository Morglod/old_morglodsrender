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

class RenderContext;
class GeometryBuffer;

class UIElement;
class UIManager;

class Rect {
    friend class UIElement;
public:
    inline glm::vec2 GetPos();
    inline glm::vec2 GetSize();

    inline void SetPos(const glm::vec2& p);
    inline void SetSize(const glm::vec2& s);

    Rect() : v2(new glm::vec2[2]) {}
    Rect(const glm::vec2& p, const glm::vec2& s) : v2(new glm::vec2[2]{p,s}) {}
    Rect(const float& px, const float& py, const float& sx, const float& sy) : v2(new glm::vec2[2]{glm::vec2(px,py),glm::vec2(sx,sy)}) {}
    ~Rect(){ delete [] v2; }

    inline bool operator == (const Rect& r) { return  ( (r.v2[0] == v2[0]) && (r.v2[1] == v2[1]) ); }
    inline bool operator != (const Rect& r) { return !( (r.v2[0] == v2[0]) && (r.v2[1] == v2[1]) ); }
protected:
    glm::vec2* v2;
};

class UIElement {
    friend class UIManager;
public:
    MR::Event<const Rect&> OnRectChanged;
    MR::Event<const glm::vec4&> OnColorChanged;
    MR::Event<const glm::vec2&> OnScaleChanged;

    virtual void SetRect(const Rect& r);
    inline Rect* GetRectPtr();

    virtual void SetColor(const glm::vec4& rgba);
    inline glm::vec4 GetColor();

    virtual void SetScale(const glm::vec2& scale);
    inline glm::vec2 GetScale();

    //Add already exist / remove from list but don't free memory
    virtual void Add(UIElement* element);
    virtual void Remove(UIElement* element);

    //Create / Free memory
    virtual UIElement* Create(MR::UIManager* m, const Rect& r);
    virtual void Delete(UIElement* element);

    virtual void Draw(RenderContext* rc, const float& delta);

    UIElement(UIManager* m, const Rect& r);
    virtual ~UIElement();
protected:
    Rect _rect;
    bool _visible;
    glm::vec4 _color;
    glm::vec2 _scale; //rect = _rect * _scale
    UIManager* _manager;

    std::vector<UIElement*> _elements;
};

typedef UIElement UILayout;

class UIManager {
    friend class UIElement;
public:
    inline void Add(UILayout* l);
    inline size_t GetLayoutsNum();
    inline UILayout* GetLayout(const size_t& i);
    inline UILayout** GetLayouts();

    virtual void Draw(RenderContext* rc, const float& delta);
    virtual void SetScreenRect(const glm::vec2& size);
    inline glm::vec2 GetScreenRect();

    static inline UIManager* Instance();

    UIManager();
    virtual ~UIManager();
protected:
    std::vector<UILayout*> _layouts;
    GeometryBuffer* _quad_geom;
    glm::mat4* _projectionMatrix;
    glm::mat4* _viewMatrix;
    glm::mat4* _mvp;
    glm::vec4* _color;
    int* _albedo_tex_unit;
    glm::vec2 _screen_size;
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

glm::vec2 MR::UIElement::GetScale(){
    return _scale;
}

void MR::UIManager::Add(UILayout* l){
    _layouts.push_back(l);
}

size_t MR::UIManager::GetLayoutsNum() {
    return _layouts.size();
}

MR::UILayout* MR::UIManager::GetLayout(const size_t& i) {
    return _layouts[i];
}

MR::UILayout** MR::UIManager::GetLayouts() {
    return _layouts.data();
}

glm::vec2 MR::UIManager::GetScreenRect(){
    return _screen_size;
}

MR::UIManager* MR::UIManager::Instance() {
    static UIManager* m = new UIManager();
    return m;
}

#endif //_MR_UI_H_
