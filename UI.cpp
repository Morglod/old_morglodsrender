#include "UI.hpp"

#include "GeometryBufferV2.hpp"
#include "Shader.hpp"
#include "RenderContext.hpp"
#include "Log.hpp"

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

const char* code_ui_vert =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "uniform	mat4 mvp;"
    "uniform	mat4 viewMatrix;"
    "uniform	mat4 projMatrix;"
    "layout (location = 0) in vec3 position;"
    "layout (location = 1) in vec3 normal;"
    "layout (location = 2) in vec4 color;"
    "layout (location = 3) in vec2 texCoord;"
    "layout (location = 0) out vec3 VertexPos;"
    "layout (location = 1) out vec3 VertexNormal;"
    "layout (location = 2) out vec4 VertexColor;"
    "layout (location = 3) out vec2 VertexTexCoord;"
    "void main() {"
    "	VertexPos = position;"
    "	VertexColor = color;"
    "	VertexNormal = normal;"
    "	VertexTexCoord = texCoord;"
    "	gl_Position = (mvp) * vec4(position,1);"
    "}";

const char* code_ui_frag_color_only =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 VertexPos;"
    "layout (location = 1) in vec4 VertexNormal;"
    "layout (location = 2) in vec4 VertexColor;"
    "layout (location = 3) in vec2 VertexTexCoord;"
    "uniform vec4 ENGINE_COLOR;"
    "void main() {"
    "	gl_FragColor = ENGINE_COLOR;"
    "}";

const char* code_ui_frag_color_texture =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 VertexPos;"
    "layout (location = 1) in vec4 VertexNormal;"
    "layout (location = 2) in vec4 VertexColor;"
    "layout (location = 3) in vec2 VertexTexCoord;"
    "uniform sampler2D ENGINE_ALBEDO;"
    "uniform vec4 ENGINE_COLOR;"
    "void main() {"
    "	gl_FragColor = texture(ENGINE_ALBEDO, VertexTexCoord) * ENGINE_COLOR;"
    "}";

namespace MR{

void UIElement::SetRect(const Rect& r){
    _rect = r;
    OnRectChanged(this, r);
}

void UIElement::SetColor(const glm::vec4& rgba){
    _color = rgba;
    OnColorChanged(this, rgba);
}

void UIElement::SetScale(const glm::vec2& scale){
    _scale = scale;
    OnScaleChanged(this, scale);
}

void UIElement::Add(IUIElement* element){
    _elements.push_back(element);
}

void UIElement::Remove(IUIElement* element){
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
    std::vector<IUIElement*>::iterator it = std::find(_elements.begin(), _elements.end(), element);
    if(it == _elements.end()) return;
    delete (*it);
    _elements.erase(it);
}

void UIElement::Draw(RenderContext* rc, const float& delta){
    if(_visible){
        glDisable(GL_DEPTH_TEST);

        *(_manager->_color) = _color;
        /*glm::mat4 modelMat = glm::translate(glm::vec3(_rect.GetPos(), 0.0f)) * glm::scale(glm::vec3(_rect.GetSize()*_scale, 1.0f));
        *(_manager->_mvp) = modelMat * (*(_manager->_projectionMatrix)) * (*(_manager->_viewMatrix));*/

        _manager->_shader->Use(rc);
        //_manager->_quad_geom->Draw();

        glBegin(GL_QUADS);
        glVertex2f(_rect.GetPos().x,                            _rect.GetPos().y);
        glVertex2f(_rect.GetPos().x+_rect.GetSize().x*_scale.x, _rect.GetPos().y);
        glVertex2f(_rect.GetPos().x+_rect.GetSize().x*_scale.x, _rect.GetPos().y+_rect.GetSize().y*_scale.y);
        glVertex2f(_rect.GetPos().x,                            _rect.GetPos().y+_rect.GetSize().y*_scale.y);
        glEnd();

        glEnable(GL_DEPTH_TEST);
    }
}

UIElement::UIElement(UIManager* m, const Rect& r) : Super(), _rect(r), _visible(true), _color(0.0f,0.0f,0.0f,1.0f), _scale(1.0f, 1.0f), _manager(m) {
}

UIElement::~UIElement(){
}

void UIManager::Draw(RenderContext* rc, const float& delta){
    for(UIElement* l : _layouts){
        l->Draw(rc, delta);
    }
}

void UIManager::SetScreenRect(const glm::vec2& size){
    *_projectionMatrix = glm::ortho(0.0f, size.x, size.y, 0.0f);
    *_viewMatrix = glm::mat4(1.0f);
    *_mvp = (*_projectionMatrix) * (*_viewMatrix);
    _screen_size = size;
}

UIManager::UIManager() : _projectionMatrix(new glm::mat4(1.0f)), _viewMatrix(new glm::mat4(1.0f)), _mvp(new glm::mat4(1.0f)), _color(new glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), _albedo_tex_unit(new int(0)), _screen_size(640.0f, 480.0f), _shader(0) {
    //_quad_geom = MR::GeometryBuffer::CreatePlane(glm::vec3(1.0f,1.0f,0.0f), glm::vec3(-0.5f, -0.5f, 0.0f), MR::GeometryBuffer::Usage::Static, MR::GeometryBuffer::DrawMode::Quads);

    _shader = new MR::Shader(MR::ShaderManager::Instance(), "UIDefaultShader", "FromMem");
    _shader->AttachSubShader(new MR::SubShader(code_ui_vert, MR::ISubShader::Type::Vertex));
    _shader->AttachSubShader(new MR::SubShader(code_ui_frag_color_only, MR::ISubShader::Type::Fragment));
    _shader->Link();
    _shader->CreateUniform("viewMatrix", _viewMatrix);
    _shader->CreateUniform("projMatrix", _projectionMatrix);
    _shader->CreateUniform("mvp", _mvp);
    _shader->CreateUniform("ENGINE_COLOR", _color);
    //_shader->CreateUniform("ENGINE_ALBEDO", _albedo_tex_unit);
}

UIManager::~UIManager(){
    for(UIElement* l : _layouts){
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

