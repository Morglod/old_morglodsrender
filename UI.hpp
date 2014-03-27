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

class RenderContext;
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

class Rect : public Super, public IRect {
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

    Rect() : Super(), v2(new glm::vec2[2]) {}
    Rect(const glm::vec2& p, const glm::vec2& s) : Super(), v2(new glm::vec2[2]{p,s}) {}
    Rect(const float& px, const float& py, const float& sx, const float& sy) : Super(), v2(new glm::vec2[2]{glm::vec2(px,py),glm::vec2(sx,sy)}) {}
    virtual ~Rect(){ delete [] v2; }
protected:
    glm::vec2* v2;
};

class IUIElement {
public:
    MR::Event<const Rect&> OnRectChanged;
    MR::Event<const glm::vec4&> OnColorChanged;
    MR::Event<const glm::vec2&> OnScaleChanged;

    virtual void SetRect(const Rect& r) = 0;
    virtual Rect* GetRectPtr() = 0;

    virtual void SetColor(const glm::vec4& rgba) = 0;
    virtual glm::vec4 GetColor() = 0;

    virtual void SetScale(const glm::vec2& scale) = 0;
    virtual glm::vec2 GetScale() = 0;

    virtual void Add(IUIElement* element) = 0;
    virtual void Remove(IUIElement* element) = 0;

    virtual void Draw(RenderContext* rc, const float& delta) = 0;

    virtual ~IUIElement(){}
};

class UIElement : public Super, public IUIElement {
    friend class UIManager;
public:
    void SetRect(const Rect& r) override;
    inline Rect* GetRectPtr() override;

    void SetColor(const glm::vec4& rgba) override;
    inline glm::vec4 GetColor() override;

    void SetScale(const glm::vec2& scale) override;
    inline glm::vec2 GetScale() override;

    //Add already exist / remove from list but don't free memory
    void Add(IUIElement* element) override;
    void Remove(IUIElement* element) override;

    //Create / Free memory
    virtual UIElement* Create(MR::UIManager* m, const Rect& r);
    virtual void Delete(UIElement* element);

    void Draw(RenderContext* rc, const float& delta) override;

    UIElement(UIManager* m, const Rect& r);
    virtual ~UIElement();
protected:
    Rect _rect;
    bool _visible;
    glm::vec4 _color;
    glm::vec2 _scale; //rect = _rect * _scale
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

    virtual void Draw(RenderContext* rc, const float& delta);
    virtual void SetScreenRect(const glm::vec2& size);
    inline glm::vec2 GetScreenRect();

    static inline UIManager* Instance();

    UIManager();
    virtual ~UIManager();
protected:
    std::vector<UIElement*> _layouts;
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

glm::vec2 MR::UIManager::GetScreenRect(){
    return _screen_size;
}

MR::UIManager* MR::UIManager::Instance() {
    static UIManager* m = new UIManager();
    return m;
}

#endif //_MR_UI_H_
