#include "Pipeline.hpp"
//#include "Scene/Scene.hpp"
#include "RenderTarget.hpp"
#include "Materials/MaterialInterfaces.hpp"
#include "Geometry/GeometryBufferV2.hpp"
#include "Shaders/ShaderBuilder.hpp"
#include "Context.hpp"
#include "RenderManager.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

MR::GeometryBuffer* __pipeline__createScreenQuad(){
    return nullptr; //MR::GeometryBuffer::Plane(glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(0,0,0), MR::IBuffer::Static+MR::IBuffer::Draw, MR::IGeometryBuffer::Draw_Quads);
}

namespace MR {

///FORWARD

bool ForwardRenderingPipeline::Setup(IContext* ctx) {
    _ctx = ctx;

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
//    if(_scene) _scene->Draw();
    if(_post_scene) _post_scene(dynamic_cast<MR::IPipeline*>(this), delta);

    return true;
}

ForwardRenderingPipeline::ForwardRenderingPipeline() : IRenderingPipeline() {
}

ForwardRenderingPipeline::~ForwardRenderingPipeline() {
}

///DEFFERED

bool DefferedRenderingPipeline::Setup(IContext* ctx) {
    _ctx = ctx;

    int win_w, win_h;
    ctx->GetWindowSizes(&win_w, &win_h);

    _gbuffer = new RenderTarget("DefferedRenderingPipeline_GBuffer", 3, win_w, win_h);
    _gbuffer->CreateTargetTexture(0, MR::ITexture::IFormat_RGB10_A2, MR::ITexture::Format_RGBA, MR::ITexture::Type_FLOAT); // scene color + nothing
    _gbuffer->CreateTargetTexture(1, MR::ITexture::IFormat_RGB10_A2, MR::ITexture::Format_RGBA, MR::ITexture::Type_FLOAT); // world normal + light model
    _gbuffer->CreateTargetTexture(2, MR::ITexture::IFormat_RGBA16, MR::ITexture::Format_RGBA, MR::ITexture::Type_FLOAT); // world pos + depth

    _plane_buf = __pipeline__createScreenQuad();
    _plane = new MR::Geometry(_plane_buf, 0);

    MR::IShaderProgram::Features shaderRequest;
    shaderRequest.ambient = true;
    shaderRequest.toScreen = true;
    shaderRequest.defferedRendering = true;

    MR::ShaderBuilder::Params reqParams;
    reqParams.features = shaderRequest;

    _rtt_shader = MR::ShaderBuilder::Need(reqParams);

    return true;
}

bool DefferedRenderingPipeline::Frame(const float& delta) {
    MR::RenderManager* rm = MR::RenderManager::GetInstance();

    rm->BindRenderTarget(_gbuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    rm->ActiveMaterialFlag(2);//TODO MR::MaterialManager::GetInstance()->ActiveFlag(2);
    FrameScene(delta);
    rm->UnBindRenderTarget();

    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    rm->SetTexture(GL_TEXTURE_2D, _gbuffer->GetTargetTexture(0), 0);
    rm->SetTexture(GL_TEXTURE_2D, _gbuffer->GetTargetTexture(1), 1);
    rm->SetTexture(GL_TEXTURE_2D, _gbuffer->GetTargetTexture(2), 2);
    rm->SetTexture(GL_TEXTURE_2D, _gbuffer->GetTargetTexture(0), 10);

    rm->ActiveMaterialFlag(2);//MR::MaterialManager::GetInstance()->ActiveFlag(0);
    rm->SetShaderProgram(_rtt_shader);
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

    _plane->Draw();

    rm->UnBindTexture(0);
    rm->UnBindTexture(1);
    rm->UnBindTexture(2);
    rm->UnBindTexture(10);

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
    if(_scene) _scene->Draw();
    if(_post_scene) _post_scene(dynamic_cast<MR::IPipeline*>(this), delta);

    return true;
}

DefferedRenderingPipeline::DefferedRenderingPipeline() : IRenderingPipeline() {
}

DefferedRenderingPipeline::~DefferedRenderingPipeline() {
}

}
