#include "UI.hpp"

#include "GeometryBuffer.hpp"
#include "Shader.hpp"
#include "RenderContext.hpp"

namespace MR{

void UIElement::SetRect(const Rect& r){
    if(_rect != r){
        _rect = r;
        OnRectChanged(this, r);
    }
}

void UIElement::SetColor(const glm::vec4& rgba){
    if( (_color) != rgba){
        _color = rgba;
        OnColorChanged(this, rgba);
    }
}

void UIElement::SetScale(const glm::vec2& scale){
    if( (_scale) != scale){
        _scale = scale;
        OnScaleChanged(this, scale);
    }
}

void UIElement::Add(UIElement* element){
    _elements.push_back(element);
}

void UIElement::Remove(UIElement* element){
    auto it = std::find(_elements.begin(), _elements.end(), element);
    if(it == _elements.end()) return;
    _elements.erase(it);
}

UIElement* UIElement::Create(MR::UIManager* m, const Rect& r){
    UIElement* p = new UIElement(m, r);
    Add(p);
    return p;
}

void UIElement::Delete(UIElement* element){
    std::vector<UIElement*>::iterator it = std::find(_elements.begin(), _elements.end(), element);
    if(it == _elements.end()) return;
    delete (*it);
    _elements.erase(it);
}

void UIElement::Draw(RenderContext* rc, const float& delta){
    if(_visible){
        const float sx = 1.0f / _manager->_screen_size.x;
        const float sy = 1.0f / _manager->_screen_size.y;

        rc->UseShader(_manager->_shader);
        *(_manager->_color) = _color;
        *(_manager->_mvp) = (glm::translate(glm::mat4(1.0f), glm::vec3(_rect.GetPos().x*sx, -_rect.GetPos().y*sy, 0.0f))
                                *
                            glm::scale(glm::mat4(1.0f), glm::vec3(_rect.GetSize(), 0.0f)))
                                *
                            (*(_manager->_projectionMatrix)) * (*(_manager->_viewMatrix));
        _manager->_quad_geom->Draw();
    }
}

UIElement::UIElement(UIManager* m, const Rect& r) : _rect(r), _visible(true), _color(0.0f,0.0f,0.0f,1.0f), _scale(1.0f, 1.0f), _manager(m) {
}

UIElement::~UIElement(){
}

void UIManager::Draw(RenderContext* rc, const float& delta){
    for(UILayout* l : _layouts){
        l->Draw(rc, delta);
    }
}

void UIManager::SetScreenRect(const glm::vec2& size){
    *_projectionMatrix = glm::ortho(-size.x/2.0f, size.x/2.0f, size.y/2.0f, -size.y/2.0f, -1.0f, 1.0f);
    *_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0,1,0));
    *_mvp = (*_projectionMatrix) * (*_viewMatrix);
    _screen_size = size;
}

UIManager::UIManager() : _projectionMatrix(new glm::mat4(glm::ortho(-320.0f, 320.0f, 240.0f, -240.0f, -1.0f, 1.0f))), _viewMatrix(new glm::mat4(1.0f)), _mvp(new glm::mat4(1.0f)), _color(new glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), _albedo_tex_unit(new int(0)), _screen_size(640.0f, 480.0f), _shader(0) {
    _quad_geom = MR::GeometryBuffer::CreatePlane(glm::vec3(1.0f,1.0f,0.0f), glm::vec3(-0.5f, -0.5f, 0.0f), MR::GeometryBuffer::Usage::Static, MR::GeometryBuffer::DrawMode::Quads);

    _shader = new MR::Shader(MR::ShaderManager::Instance(), "UIDefaultShader", "FromMem");
    _shader->AttachSubShader(MR::SubShader::DefaultVert());
    _shader->AttachSubShader(MR::SubShader::DefaultFrag());
    _shader->Link();
    _shader->CreateUniform("viewMatrix", _viewMatrix);
    _shader->CreateUniform("projMatrix", _projectionMatrix);
    _shader->CreateUniform("mvp", _mvp);
    _shader->CreateUniform("ENGINE_COLOR", _color);
    _shader->CreateUniform("ENGINE_ALBEDO", _albedo_tex_unit);
}

UIManager::~UIManager(){
    for(UILayout* l : _layouts){
        delete l;
    }
    _layouts.clear();
    delete _shader;
    delete _quad_geom;
    delete _projectionMatrix;
    delete _viewMatrix;
    delete _color;
    delete _albedo_tex_unit;
}

}

