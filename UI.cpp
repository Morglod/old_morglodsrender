#include "UI.hpp"

#include "GeometryBufferV2.hpp"
#include "Shader.hpp"
#include "RenderSystem.hpp"
#include "Log.hpp"

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

const char* code_ui_vert =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "uniform mat4 modelMatrix;"
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
    "	gl_Position = (modelMatrix) * vec4(position,1);"
    "}";

const char* code_ui_frag_color_only =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 VertexPos;"
    "layout (location = 1) in vec4 VertexNormal;"
    "layout (location = 2) in vec4 VertexColor;"
    "layout (location = 3) in vec2 VertexTexCoord;"
    "out vec4 fragColor;"
    "uniform vec4 ENGINE_COLOR;"
    "void main() {"
    "	fragColor = ENGINE_COLOR;"
    "}";

const char* code_ui_frag_color_texture =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 VertexPos;"
    "layout (location = 1) in vec4 VertexNormal;"
    "layout (location = 2) in vec4 VertexColor;"
    "layout (location = 3) in vec2 VertexTexCoord;"
    "out vec4 fragColor;"
    "uniform sampler2D ENGINE_ALBEDO;"
    "uniform vec4 ENGINE_COLOR;"
    "void main() {"
    "	fragColor = texture(ENGINE_ALBEDO, VertexTexCoord) * ENGINE_COLOR;"
    "}";

namespace MR{

void UIElement::SetRect(const Rect& r){
    _rect = r;
    _CalcMatrix();
    OnRectChanged(this, r);
}

void UIElement::SetColor(const glm::vec4& rgba){
    _color = rgba;
    OnColorChanged(this, rgba);
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

void UIElement::Draw(IRenderSystem* rc, const float& delta){
    Draw(rc, delta, nullptr, glm::mat4(1.0f));
}

void UIElement::Draw(IRenderSystem* rc, const float& delta, IUIElement* parent, const glm::mat4& parent_mat){
    if(_visible){
        glDisable(GL_DEPTH_TEST);

        *(_manager->_color) = _color;

        *(_manager->_modelMatrix) = *_model_mat;

        _manager->_shader->Use(rc);
        _manager->_quad_geom->Draw(rc);

        glEnable(GL_DEPTH_TEST);

        for(auto it = _elements.begin(); it != _elements.end(); ++it){
            (*it)->Draw(rc, delta, this, (*_model_mat));
        }
    }
}

void UIElement::_CalcMatrix(){
    glm::mat4 t = glm::translate(_rect.GetPos().x/100.0f*2.0f-1.0f, _rect.GetPos().y/-100.0f*2.0f+1.0f, 0.0f);
    glm::mat4 s = glm::scale(_rect.GetSize().x/100.0f*2.0f, _rect.GetSize().y/100.0f*2.0f, 0.0f);
    *_model_mat = t * s;

    glm::mat4 parent_t = glm::translate(_rect.GetPos().x/100.0f, _rect.GetPos().y/100.0f, 0.0f);
    glm::mat4 parent_s = glm::scale(_rect.GetSize().x/100.0f, _rect.GetSize().y/100.0f, 1.0f);
    *_model_mat_for_children = parent_t * parent_s;
}

UIElement::UIElement(UIManager* m, const Rect& r) : Object(), _rect(r), _visible(true), _color(0.0f,0.0f,0.0f,1.0f), _model_mat(new glm::mat4(1.0f)), _model_mat_for_children(new glm::mat4(1.0f)), _manager(m) {
    _CalcMatrix();
}

UIElement::~UIElement(){
    delete _model_mat;
    delete _model_mat_for_children;
}

void UIManager::Draw(IRenderSystem* rc, const float& delta){
    for(UIElement* l : _layouts){
        l->Draw(rc, delta);
    }
}

UIManager::UIManager()
 :
_modelMatrix(new glm::mat4(1.0f)),
_color(new glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), _tex_unit(new int(0)), _shader(0)

{
    _quad_geom_buffer = MR::GeometryBuffer::Plane(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f), MR::IGLBuffer::Static+MR::IGLBuffer::Draw, MR::IGeometryBuffer::Draw_Quads);
    _quad_geom = new MR::Geometry(_quad_geom_buffer, 0, 4, 4);

    _shader = new MR::Shader(MR::ShaderManager::Instance(), "UIDefaultShader", "FromMem");
    _shader->AttachSubShader(new MR::SubShader(code_ui_vert, MR::ISubShader::Type::Vertex));
    _shader->AttachSubShader(new MR::SubShader(code_ui_frag_color_only, MR::ISubShader::Type::Fragment));
    _shader->Link();
    _shader->CreateUniform("modelMatrix", _modelMatrix);
    _shader->CreateUniform("ENGINE_COLOR", _color);
    _shader->CreateUniform("ENGINE_ALBEDO", _tex_unit);
}

UIManager::~UIManager(){
    for(UIElement* l : _layouts){
        delete l;
    }
    _layouts.clear();
    delete _shader;
    delete _quad_geom;
    delete _quad_geom_buffer;
    delete _modelMatrix;
    delete _color;
    delete _tex_unit;
}

}

