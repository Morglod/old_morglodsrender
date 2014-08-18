#include "ScreenQuad.hpp"
#include "../Shaders/ShaderBuilder.hpp"
#include "../Shaders/ShaderInterfaces.hpp"
#include "../Geometry/GeometryBufferV2.hpp"
#include "../RenderManager.hpp"
#include "../Materials/MaterialsConfig.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

namespace MR {

void ScreenQuad::Draw() {
    MR::RenderManager* rm = MR::RenderManager::GetInstance();
    rm->SetActivedMaterialPass(nullptr);
    if(_prog) rm->SetShaderProgram(_prog);
    if(_texture) {
        rm->SetTexture(_texture, _ambientTexUnit);
        _prog->UpdateUniforms();
    }
    if(_quad) {
        glDisable(GL_CULL_FACE);
        _quad->Draw();
    }
}

ScreenQuad::ScreenQuad() {
    _ambientTexUnit = MR_MATERIAL_EMISSION_TEX_DEFAULT;

    _quad = MR::Geometry::MakeQuad(glm::vec2(2, 2), glm::vec3(-1.0f, -1.0f, 0.0f), true);
    _texture = nullptr;

    MR::ShaderBuilder::Params params;
    params.features.light = false;
    params.features.ambient = true;
    params.features.toScreen = true;

    _prog = MR::ShaderBuilder::Need(params);
    _ambientTex = _prog->CreateUniform(MR_SHADER_AMBIENT_TEX, MR::IShaderUniform::Type::SUT_Sampler2D, &_ambientTexUnit);
}

ScreenQuad::~ScreenQuad() {
    delete _quad;
}

}
