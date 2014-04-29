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

class Rect {
public:
    glm::vec4 data;

    inline void SetPos(const glm::vec2& pos) { data[0] = pos.x; data[1] = pos.y; }
    inline void SetSize(const glm::vec2& sz) { data[2] = sz.x; data[3] = sz.y; }
    inline glm::vec2 GetPos() { return glm::vec2(data[0], data[1]); }
    inline glm::vec2 GetSize() { return glm::vec2(data[2], data[3]); }

    inline bool TestPoint(const glm::vec2& point){
        if(point.x < data.x) return false;
        if(point.y < data.y) return false;
        if(point.x > (data.x+data.z)) return false;
        if(point.y > (data.y+data.w)) return false;
        return true;
    }

    Rect(const float& px, const float& py, const float& sx, const float& sy) : data(glm::vec4(px, py, sx, sy)) {}
    Rect(const glm::vec4& v) : data(v) {}
};

class IUIElement {
public:
    MR::EventListener<IUIElement*, const Rect&> OnRectChanged;
    MR::EventListener<IUIElement*, const glm::vec4&> OnColorChanged;
    MR::EventListener<IUIElement*, const glm::vec2&> OnScaleChanged;
    MR::EventListener<IUIElement*, const glm::vec2&, const bool&> OnMouseHoverStateChanged; //pos, state (hover / not hover)
    MR::EventListener<IUIElement*, const glm::vec2&, const int&, const bool&> OnMouseHoverDownStateChanged; //pos, glfw mouse button (GLFW_MOUSE_BUTTON_LEFT GLFW_MOUSE_BUTTON_MIDDLE GLFW_MOUSE_BUTTON_RIGHT)

    virtual void SetRect(const Rect& r) = 0;
    virtual Rect GetRect() = 0;

    virtual void SetColor(const glm::vec4& rgba) = 0;
    virtual glm::vec4 GetColor() = 0;

    virtual void SetHoverState(const bool& state) = 0;
    virtual bool GetHoverState() = 0;

    virtual void SetClickState(const bool& state) = 0;
    virtual bool GetClickState() = 0;

    virtual void Frame(IRenderSystem* rc, const float& delta) = 0;

    virtual ~IUIElement() {}
};

class UIElement : public Object, public IUIElement {
    friend class UIManager;
public:
    void SetRect(const Rect& r) override;
    inline Rect GetRect() override { return _rect; }

    void SetColor(const glm::vec4& rgba) override;
    inline glm::vec4 GetColor() override { return _color; }

    void SetHoverState(const bool& state) override { _hover = state; }
    bool GetHoverState() override { return _hover; }

    void SetClickState(const bool& state) override { _click = state; }
    bool GetClickState() override { return _click; }

    void Frame(IRenderSystem* rc, const float& delta) override;

    UIElement(UIManager* m, const Rect& r);
    virtual ~UIElement();
protected:
    void _CalcMatrix();

    Rect _rect;
    bool _visible;
    glm::vec4 _color;
    glm::mat4* _model_mat;
    UIManager* _manager;

    bool _hover, _hover_state;
    bool _click, _mouse_down[3];

    std::vector<IUIElement*> _elements;
};

class UIManager {
    friend class UIElement;
public:
    inline void Add(IUIElement* l) { _layouts.push_back(l); }
    inline size_t GetLayoutsNum() { return _layouts.size(); }
    inline IUIElement* GetLayout(const size_t& i) { return _layouts[i]; }
    inline IUIElement** GetLayouts() { return _layouts.data(); }

    virtual void Frame(IRenderSystem* rc, const float& delta);

    static inline UIManager* Instance() {
        static UIManager* m = new UIManager();
        return m;
    }

    UIManager();
    virtual ~UIManager();
protected:
    std::vector<IUIElement*> _layouts;
    GeometryBuffer* _quad_geom_buffer;
    IGeometry* _quad_geom;
    glm::mat4* _modelMatrix;
    glm::vec4* _color;
    int* _tex_unit;
    MR::Shader* _shader;
};

}

#endif //_MR_UI_H_
