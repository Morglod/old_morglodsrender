#include "UI.hpp"

#include "GeometryBufferV2.hpp"
#include "Shaders/ShaderInterfaces.hpp"
#include "RenderSystem.hpp"
#include "RenderWindow.hpp"
#include "Utils/Log.hpp"

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

#include "Shaders/ShaderCompiler.hpp"
#include "Shaders/ShaderObjects.hpp"

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

void UIElement::Frame(IRenderSystem* rc, const float& delta){
    if(_visible){
        if(_hover){
            int ww, wh;
            rc->GetWindow()->GetSize(&ww, &wh);

            double mousex, mousey;
            rc->GetWindow()->GetMousePos(&mousex, &mousey);
            glm::vec2 mp(mousex / (ww/100.0f), mousey / (wh/100.0f));

            if(GetRect().TestPoint(glm::vec2(mp))) {
                if(!_hover_state) OnMouseHoverStateChanged.Call(this, mp, true);
                if(_click){
                    if(rc->GetWindow()->GetMouseButtonDown(0)) {
                        if(!_mouse_down[0])
                            OnMouseHoverDownStateChanged.Call(this, mp, 0, true);
                        _mouse_down[0] = true;
                    } else if(_mouse_down[0]) {
                        OnMouseHoverDownStateChanged.Call(this, mp, 0, false);
                        _mouse_down[0] = false;
                    }

                    if(rc->GetWindow()->GetMouseButtonDown(1)) {
                        if(!_mouse_down[1])
                            OnMouseHoverDownStateChanged.Call(this, mp, 1, true);
                        _mouse_down[1] = true;
                    } else if(_mouse_down[1]) {
                        OnMouseHoverDownStateChanged.Call(this, mp, 1, false);
                        _mouse_down[1] = false;
                    }

                    if(rc->GetWindow()->GetMouseButtonDown(2)) {
                        if(!_mouse_down[2])
                            OnMouseHoverDownStateChanged.Call(this, mp, 2, true);
                        _mouse_down[2] = true;
                    } else if(_mouse_down[2]) {
                        OnMouseHoverDownStateChanged.Call(this, mp, 2, false);
                        _mouse_down[2] = false;
                    }
                }
            } else {
                if(_hover_state) _hover_state = false;
                OnMouseHoverStateChanged.Call(this, mp, false);
            }
        }

        glDisable(GL_DEPTH_TEST);

        if(_manager){
            *(_manager->_color) = _color;
            *(_manager->_modelMatrix) = *_model_mat;
            *(_manager->_tex_unit) = tex_unit;

            rc->UseShaderProgram(_manager->_shader);
            _manager->_quad_geom->Draw(rc);
        }

        glEnable(GL_DEPTH_TEST);
    }
}

void UIElement::_CalcMatrix(){
    glm::mat4 t = glm::translate(_rect.GetPos().x/100.0f*2.0f-1.0f, _rect.GetPos().y/-100.0f*2.0f+1.0f, 0.0f);
    glm::mat4 s = glm::scale(_rect.GetSize().x/100.0f*2.0f, _rect.GetSize().y/100.0f*2.0f, 0.0f);
    *_model_mat = t * s;
}

UIElement::UIElement(UIManager* m, const Rect& r) : _rect(r), _visible(true), _color(0.0f,0.0f,0.0f,1.0f), _model_mat(new glm::mat4(1.0f)), _manager(m), _hover(true), _hover_state(false), _click(true), _mouse_down { false, false, false} {
    _CalcMatrix();
}

UIElement::~UIElement(){
    delete _model_mat;
}

void UIManager::Frame(IRenderSystem* rc, const float& delta){
    for(IUIElement* l : _layouts){
        l->Frame(rc, delta);
    }
}

UIManager::UIManager()
 :
_modelMatrix(new glm::mat4(1.0f)),
_color(new glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), _tex_unit(new int(0)), _shader(0)

{
    _quad_geom_buffer = MR::GeometryBuffer::Plane(GL::GetCurrent(), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f), MR::IBuffer::Static+MR::IBuffer::Draw, MR::IGeometryBuffer::Draw_Quads);
    _quad_geom = new MR::Geometry(_quad_geom_buffer, 0, 0, 4, 4);

    MR::ShaderCompiler compiler;
    _shader = MR::ShaderProgram::Create();//new MR::ShaderProgram(MR::ShaderManager::Instance(), "UIDefaultShader", "FromMem");
    MR::IShader* vert, * frag;
    vert = MR::Shader::Create(IShader::ST_Vertex);
    frag = MR::Shader::Create(IShader::ST_Fragment);

    compiler.Compile(code_ui_vert, IShaderCompiler::ST_Vertex, vert->GetGPUHandle());
    compiler.Compile(code_ui_frag_color_only, IShaderCompiler::ST_Fragment, frag->GetGPUHandle());

    vert->Attach(_shader);
    frag->Attach(_shader);

    //_shader->AttachSubShader(new MR::SubShader(code_ui_vert, MR::ISubShader::Type::Vertex));
    //_shader->AttachSubShader(new MR::SubShader(code_ui_frag_color_only, MR::ISubShader::Type::Fragment));

    compiler.Link(StaticArray<unsigned int>(), _shader->GetGPUHandle());

    //_shader->Link();
    _shader->CreateUniform("modelMatrix", MR::IShaderUniform::SUT_Mat4, _modelMatrix);
    _shader->CreateUniform("ENGINE_COLOR", MR::IShaderUniform::SUT_Vec4, _color);
    _shader->CreateUniform("ENGINE_ALBEDO", MR::IShaderUniform::SUT_Int, _tex_unit);
}

UIManager::~UIManager(){
    for(IUIElement* l : _layouts){
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

