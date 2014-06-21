#pragma once

#ifndef _MR_PIPELINE_H_
#define _MR_PIPELINE_H_

namespace MR {

class IRenderSystem;
namespace GL { class IContext; }

class IPipeline {
public:
    virtual bool Setup(IRenderSystem* rs, GL::IContext* ctx) = 0;
    virtual bool Frame(const float& delta) = 0;
    virtual void Shutdown() = 0;

    virtual bool FrameScene(const float& delta) = 0;

    IPipeline() {}
    virtual ~IPipeline() {}
};

class IRenderingPipeline : public IPipeline {
public:
    IRenderingPipeline() : IPipeline() {}
    virtual ~IRenderingPipeline() {}
};

/* Good for 2D scenes, without builtin lights */
class ForwardRenderingPipeline : public IRenderingPipeline {
public:
    bool Setup(IRenderSystem* rs, GL::IContext* ctx) override;
    bool Frame(const float& delta) override;
    void Shutdown() override;

    bool FrameScene(const float& delta) override;

    ForwardRenderingPipeline();
    virtual ~ForwardRenderingPipeline();
};

/* Good for massive scenes, with many light sources */
class DefferedRenderingPipeline : public IRenderingPipeline {
public:
    bool Setup(IRenderSystem* rs, GL::IContext* ctx) override;
    bool Frame(const float& delta) override;
    void Shutdown() override;

    bool FrameScene(const float& delta) override;

    DefferedRenderingPipeline();
    virtual ~DefferedRenderingPipeline();
};

}

#endif // _MR_PIPELINE_H_
