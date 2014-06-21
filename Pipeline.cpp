#include "Pipeline.hpp"

namespace MR {

///FORWARD

bool ForwardRenderingPipeline::Setup(IRenderSystem* rs, GL::IContext* ctx) {
    return true;
}

bool ForwardRenderingPipeline::Frame(const float& delta) {
    return true;
}

void ForwardRenderingPipeline::Shutdown() {
}

bool ForwardRenderingPipeline::FrameScene(const float& delta) {
}

ForwardRenderingPipeline::ForwardRenderingPipeline() : IRenderingPipeline() {
}

ForwardRenderingPipeline::~ForwardRenderingPipeline() {
}

///DEFFERED

bool DefferedRenderingPipeline::Setup(IRenderSystem* rs, GL::IContext* ctx) {
    return true;
}

bool DefferedRenderingPipeline::Frame(const float& delta) {
    return true;
}

void DefferedRenderingPipeline::Shutdown() {
}

bool DefferedRenderingPipeline::FrameScene(const float& delta) {
}

DefferedRenderingPipeline::DefferedRenderingPipeline() : IRenderingPipeline() {
}

DefferedRenderingPipeline::~DefferedRenderingPipeline() {
}

}
