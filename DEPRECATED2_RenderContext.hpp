/*
    RenderContext used for gpu objects management
    Can be only one instance
*/

#pragma once

#ifndef _MR_RENDER_CONTEXT_H_
#define _MR_RENDER_CONTEXT_H_

namespace MR {

class RenderContext {
public:
private:
    RenderContext();
    ~RenderContext();

    RenderContext(const RenderContext&){}
    RenderContext& operator=(const RenderContext&) {}
};

}

#endif // _MR_RENDER_CONTEXT_H_
