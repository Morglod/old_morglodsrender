#pragma once

#ifndef _MR_RENDER_TECHNIQUE_H_
#define _MR_RENDER_TECHNIQUE_H_

#include "Types.hpp"

namespace MR {

class RenderContext;

class IRenderTechnique {
public:
    virtual void SetDepthTest(const bool& state) = 0;
    virtual bool GetDepthTest() = 0;

    virtual void SetCullFace(const bool& state) = 0;
    virtual bool GetCullFace() = 0;

    virtual void Use(RenderContext* rc) = 0;
};

class RenderTechnique : public Object, public IRenderTechnique {
public:
    inline void SetDepthTest(const bool& state) override { _depth_test = state; }
    inline bool GetDepthTest() override { return _depth_test; }

    inline void SetCullFace(const bool& state) override { _cull_face = state; }
    inline bool GetCullFace() override { return _cull_face; }

    void Use(RenderContext* rc) override;
protected:
    bool _depth_test = false;
    bool _cull_face = false;
};

}

#endif // _MR_RENDER_TECHNIQUE_H_
