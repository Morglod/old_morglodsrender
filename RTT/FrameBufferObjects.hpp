#pragma once

#include "FrameBufferInterfaces.hpp"

namespace mr {

class FrameBuffer : public IFrameBuffer {
public:
    bool Create() override;
    void Destroy() override;

    void Bind(BindTarget const& target) override;
    IFrameBuffer* ReBind(BindTarget const& target) override;
    inline BindTarget GetBindTarget() override { return _bindedTarget; }

    CompletionStatus CheckCompletion(BindTarget const& target) override;

    bool SetTextureMipmap(class ITexture* tex, Attachment const& attachment, unsigned int const& mipmapLevel) override;
    bool SetTextureMipmapToColor(class ITexture* tex, unsigned int const& colorSlot, unsigned int const& mipmapLevel) override;
    bool SetRenderBuffer(class IRenderBuffer* renderBuffer, Attachment const& attachment) override;
    bool SetRenderBufferToColor(class IRenderBuffer* renderBuffer, unsigned int const& colorSlot) override;

    virtual ~FrameBuffer();
protected:
    BindTarget _bindedTarget;
};

IFrameBuffer* FrameBufferGetBinded(IFrameBuffer::BindTarget const& target);
void FrameBufferUnBind(IFrameBuffer::BindTarget const& target);

}
