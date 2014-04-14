#pragma once

#ifndef _MR_RENDER_SYSTEM_H_
#define _MR_RENDER_SYSTEM_H_

#include "Types.hpp"

namespace MR {

class IRenderSystem {
public:
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

    /* Is Initializated and not shutdowned */
    virtual bool Alive() = 0;
};

/** BASE RENDER SYSTEM CALLS THIS METHODS AUTOMATICALLY **/

/* New RenderSystem should be registered and unregistered before deleting */
void RegisterRenderSystem(IRenderSystem* rs);
void UnRegisterRenderSystem(IRenderSystem* rs);

/* Should be called after Init and Shutdown methods */
void RenderSystemInitializated(IRenderSystem* rs);
void RenderSystemShutdowned(IRenderSystem* rs);

/* Check if any render system is alive */
bool AnyRenderSystemAlive();

/** -- **/

/* BASE RENDER SYSTEM IMPLEMENTION */
class RenderSystem : public MR::IRenderSystem, public MR::Object {
public:
    bool Init() override;
    void Shutdown() override;
    inline bool Alive() override { return _alive; }
    inline std::string ToString() override { return "RenderSystem(base)"; }

    RenderSystem();
    ~RenderSystem();
protected:
    bool _alive;
    bool _glew;
};

}

#endif // _MR_RENDER_SYSTEM_H_
