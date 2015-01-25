#pragma once

#include "../Utils/Memory.hpp"
#include "../CoreObjects.hpp"

namespace mr {

class ITexture;
class IRenderBuffer;

class IFrameBuffer : public IGPUObjectHandle {
public:
    enum BindTarget {
        DrawFramebuffer = 0,
        ReadFramebuffer = 1,
        DrawReadFramebuffer,

        NotBinded = 255
    };

    enum CompletionStatus {
        Complete = 0x8CD5,
        IncompleteAttachment = 0x8CD6,
        IncompleteMissingAttachment = 0x8CD7,
        IncompleteDrawBuffer = 0x8CDB,
        IncompleteReadBuffer = 0x8CDC,
        Unsupported = 0x8CDD,
        IncompleteMultisample = 0x8D56,
        IncompleteLayerTargets = 0x8DA8
    };

    enum Attachment {
        Depth = 0x8D00,
        Stencil = 0x8D20,
        DepthStencil = 0x821A
    };

    static std::string CompletionStatusToString(CompletionStatus const& cs);

    virtual bool Create() = 0;
    virtual void Bind(BindTarget const& target) = 0;
    virtual IFrameBuffer* ReBind(BindTarget const& target) = 0;
    virtual BindTarget GetBindTarget() = 0;

    virtual CompletionStatus CheckCompletion(BindTarget const& target) = 0;

    virtual bool SetTextureMipmap(ITexture* tex, Attachment const& attachment, unsigned int const& mipmapLevel) = 0;
    virtual bool SetTextureMipmapToColor(ITexture* tex, unsigned int const& colorSlot, unsigned int const& mipmapLevel) = 0;
    virtual bool SetRenderBuffer(IRenderBuffer* renderBuffer, Attachment const& attachment) = 0;
    virtual bool SetRenderBufferToColor(IRenderBuffer* renderBuffer, unsigned int const& colorSlot) = 0;

    virtual ~IFrameBuffer() {}
};

}
