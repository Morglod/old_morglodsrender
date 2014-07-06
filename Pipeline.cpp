#include "Pipeline.hpp"
#include "Scene/Scene.hpp"
#include "RenderWindow.hpp"
#include "RenderTarget.hpp"
#include "Texture.hpp"
#include "RenderSystem.hpp"
#include "Material.hpp"
#include "Buffers/GeometryBufferV2.hpp"

#include "Shaders/ShaderBuilder.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

MR::GeometryBuffer* __pipeline__createScreenQuad(MR::GL::IContext* ctx){
    return MR::GeometryBuffer::Plane(ctx, glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(0,0,0), MR::IBuffer::Static+MR::IBuffer::Draw, MR::IGeometryBuffer::Draw_Quads);
}

namespace MR {

///FORWARD

bool ForwardRenderingPipeline::Setup(IRenderSystem* rs, GL::IContext* ctx, IRenderWindow* rw) {
    _rs = rs;
    _ctx = ctx;
    _rw = rw;

    return true;
}

bool ForwardRenderingPipeline::Frame(const float& delta) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    FrameScene(delta);
    return true;
}

void ForwardRenderingPipeline::Shutdown() {
}

bool ForwardRenderingPipeline::FrameScene(const float& delta) {
    if(_pre_scene) _pre_scene(dynamic_cast<MR::IPipeline*>(this), delta);
    if(_scene) _scene->Draw(_rs);
    if(_post_scene) _post_scene(dynamic_cast<MR::IPipeline*>(this), delta);

    return true;
}

ForwardRenderingPipeline::ForwardRenderingPipeline() : IRenderingPipeline() {
}

ForwardRenderingPipeline::~ForwardRenderingPipeline() {
}

///DEFFERED

bool DefferedRenderingPipeline::Setup(IRenderSystem* rs, GL::IContext* ctx, IRenderWindow* rw) {
    _rs = rs;
    _ctx = ctx;
    _rw = rw;

    int win_w, win_h;
    rw->GetSize(&win_w, &win_h);

    _gbuffer = new RenderTarget("DefferedRenderingPipeline_GBuffer", 3, win_w, win_h);
    _gbuffer->CreateTargetTexture(0, MR::ITexture::InternalFormat::RGB10_A2, MR::ITexture::Format::RGBA, MR::ITexture::Type::FLOAT); // scene color + nothing
    _gbuffer->CreateTargetTexture(1, MR::ITexture::InternalFormat::RGB10_A2, MR::ITexture::Format::RGBA, MR::ITexture::Type::FLOAT); // world normal + light model
    _gbuffer->CreateTargetTexture(2, MR::ITexture::InternalFormat::RGBA16, MR::ITexture::Format::RGBA, MR::ITexture::Type::FLOAT); // world pos + depth

    _plane_buf = __pipeline__createScreenQuad(_ctx);
    _plane = new MR::Geometry(_plane_buf, 0, 0, 4, 4);

    MR::IShaderProgram::Features shaderRequest;
    shaderRequest.ambient = true;
    shaderRequest.toScreen = true;
    shaderRequest.defferedRendering = true;
    _rtt_shader = MR::ShaderBuilder::Need(shaderRequest);

    return true;
}

bool DefferedRenderingPipeline::Frame(const float& delta) {
    _rs->BindRenderTarget(_gbuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    MR::MaterialManager::Instance()->ActiveFlag(2);
    FrameScene(delta);
    _rs->UnBindRenderTarget();

    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    _rs->BindTexture(GL_TEXTURE_2D, _gbuffer->GetTargetTexture(0), 0);
    _rs->BindTexture(GL_TEXTURE_2D, _gbuffer->GetTargetTexture(1), 1);
    _rs->BindTexture(GL_TEXTURE_2D, _gbuffer->GetTargetTexture(2), 2);
    _rs->BindTexture(GL_TEXTURE_2D, _gbuffer->GetTargetTexture(0), 10);

    MR::MaterialManager::Instance()->ActiveFlag(0);
    _rs->UseShaderProgram(_rtt_shader);
    glProgramUniform1i(_rtt_shader->GetGPUHandle(), glGetUniformLocation(_rtt_shader->GetGPUHandle(), MR_SHADER_UNIFORM_GBUFFER_1), 0);
    glProgramUniform1i(_rtt_shader->GetGPUHandle(), glGetUniformLocation(_rtt_shader->GetGPUHandle(), MR_SHADER_UNIFORM_GBUFFER_2), 1);
    glProgramUniform1i(_rtt_shader->GetGPUHandle(), glGetUniformLocation(_rtt_shader->GetGPUHandle(), MR_SHADER_UNIFORM_GBUFFER_3), 2);

    {
        unsigned int pointLightsNum = 0, dirLightsNum = 0;

        std::vector<MR::ILightSource*>* _ls = _scene->GetLightsListPtr();
        for(size_t li = 0; li < _ls->size(); ++li) {
            if(_ls[0][li]->GetType() == MR::ILightSource::Type::Point) {
                ++pointLightsNum;

                //POS
                glUniform3f(glGetUniformLocation(_rtt_shader->GetGPUHandle(), (std::string(MR_SHADER_POINT_LIGHTS)+"["+std::to_string((int)li)+"].pos").c_str()), _ls[0][li]->GetPos().x, _ls[0][li]->GetPos().y, _ls[0][li]->GetPos().z);

                //EMISSION
                glUniform3f(glGetUniformLocation(_rtt_shader->GetGPUHandle(), (std::string(MR_SHADER_POINT_LIGHTS)+"["+std::to_string((int)li)+"].emission").c_str()), _ls[0][li]->GetEmission().x, _ls[0][li]->GetEmission().y, _ls[0][li]->GetEmission().z);

                //AMBIENT
                glUniform3f(glGetUniformLocation(_rtt_shader->GetGPUHandle(), (std::string(MR_SHADER_POINT_LIGHTS)+"["+std::to_string((int)li)+"].ambient").c_str()), _ls[0][li]->GetAmbient().x, _ls[0][li]->GetAmbient().y, _ls[0][li]->GetAmbient().z);

                //RADIUS
                glUniform1f(glGetUniformLocation(_rtt_shader->GetGPUHandle(), (std::string(MR_SHADER_POINT_LIGHTS)+"["+std::to_string((int)li)+"].radius").c_str()), _ls[0][li]->GetRadius());

                //ATTENUATION
                glUniform1f(glGetUniformLocation(_rtt_shader->GetGPUHandle(), (std::string(MR_SHADER_POINT_LIGHTS)+"["+std::to_string((int)li)+"].attenuation").c_str()), _ls[0][li]->GetAttenuation());

                //POWER
                glUniform1f(glGetUniformLocation(_rtt_shader->GetGPUHandle(), (std::string(MR_SHADER_POINT_LIGHTS)+"["+std::to_string((int)li)+"].power").c_str()), _ls[0][li]->GetPower());
            }
        }

        glUniform1i(glGetUniformLocation(_rtt_shader->GetGPUHandle(), MR_SHADER_POINT_LIGHTS_NUM), pointLightsNum);

        size_t pointLightsLoop = 0;
        for(size_t li = 0; li < _ls->size(); ++li) {
            if(_ls[0][li]->GetType() == MR::ILightSource::Type::Point) ++pointLightsLoop;
            if(_ls[0][li]->GetType() == MR::ILightSource::Type::Dir) {
                ++dirLightsNum;

                size_t lindex = li - pointLightsLoop;

                //DIR
                glUniform3f(glGetUniformLocation(_rtt_shader->GetGPUHandle(), (std::string(MR_SHADER_DIR_LIGHTS)+"["+std::to_string((int)lindex)+"].dir").c_str()), _ls[0][li]->GetDir().x, _ls[0][li]->GetDir().y, _ls[0][li]->GetDir().z);

                //EMISSION
                glUniform3f(glGetUniformLocation(_rtt_shader->GetGPUHandle(), (std::string(MR_SHADER_DIR_LIGHTS)+"["+std::to_string((int)lindex)+"].emission").c_str()), _ls[0][li]->GetEmission().x, _ls[0][li]->GetEmission().y, _ls[0][li]->GetEmission().z);

                //AMBIENT
                glUniform3f(glGetUniformLocation(_rtt_shader->GetGPUHandle(), (std::string(MR_SHADER_DIR_LIGHTS)+"["+std::to_string((int)lindex)+"].ambient").c_str()), _ls[0][li]->GetAmbient().x, _ls[0][li]->GetAmbient().y, _ls[0][li]->GetAmbient().z);
            }
        }

        glUniform1i(glGetUniformLocation(_rtt_shader->GetGPUHandle(), MR_SHADER_DIR_LIGHTS_NUM), dirLightsNum);
    }

    _plane->Draw(_rs);

    _rs->UnBindTexture(0);
    _rs->UnBindTexture(1);
    _rs->UnBindTexture(2);
    _rs->UnBindTexture(10);

    glEnable(GL_CULL_FACE);

    return true;
}

void DefferedRenderingPipeline::Shutdown() {
    delete _gbuffer;
    _gbuffer = nullptr;

    delete _plane;
    _plane = nullptr;
}

bool DefferedRenderingPipeline::FrameScene(const float& delta) {
    if(_pre_scene) _pre_scene(dynamic_cast<MR::IPipeline*>(this), delta);
    if(_scene) _scene->Draw(_rs);
    if(_post_scene) _post_scene(dynamic_cast<MR::IPipeline*>(this), delta);

    return true;
}

DefferedRenderingPipeline::DefferedRenderingPipeline() : IRenderingPipeline() {
}

DefferedRenderingPipeline::~DefferedRenderingPipeline() {
}

}
