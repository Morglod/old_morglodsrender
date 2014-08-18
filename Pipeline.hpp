#pragma once

#ifndef _MR_PIPELINE_H_
#define _MR_PIPELINE_H_

namespace MR {

class SceneManager;
class RenderTarget;
class GeometryBuffer;
class IGeometry;
class IShaderProgram;
class IContext;

class IPipeline {
public:
    typedef bool (*FrameProc)(IPipeline* pipeline, const float& delta);

    virtual bool Setup(IContext* ctx) = 0;
    virtual bool Frame(const float& delta) = 0;
    virtual void Shutdown() = 0;

    virtual bool FrameScene(const float& delta) = 0;

    virtual void SetPreSceneFrame(FrameProc fp) { _pre_scene = fp; }
    virtual void SetPostSceneFrame(FrameProc fp) { _post_scene = fp; }

    IPipeline() {}
    virtual ~IPipeline() {}

protected:
    FrameProc _pre_scene = nullptr, _post_scene = nullptr;
    IContext* _ctx;
};

class IRenderingPipeline : public IPipeline {
public:
    virtual void SetScene(SceneManager* scene) { _scene = scene; }
    virtual SceneManager* GetScene() { return _scene; }

    IRenderingPipeline() : IPipeline() {}
    virtual ~IRenderingPipeline() {}
protected:
    SceneManager* _scene;
};

/** Good for 2D scenes, without builtin lights **/
class ForwardRenderingPipeline : public IRenderingPipeline {
public:
    bool Setup(IContext* ctx) override;
    bool Frame(const float& delta) override;
    void Shutdown() override;

    bool FrameScene(const float& delta) override;

    ForwardRenderingPipeline();
    virtual ~ForwardRenderingPipeline();
};

/** Good for massive scenes, with many light sources **/
class DefferedRenderingPipeline : public IRenderingPipeline {
public:
    bool Setup(IContext* ctx) override;
    bool Frame(const float& delta) override;
    void Shutdown() override;

    bool FrameScene(const float& delta) override;

    DefferedRenderingPipeline();
    virtual ~DefferedRenderingPipeline();
protected:
    MR::RenderTarget* _gbuffer;
    MR::GeometryBuffer* _plane_buf;
    MR::IGeometry* _plane;
    MR::IShaderProgram* _rtt_shader;
};

}

#endif // _MR_PIPELINE_H_
