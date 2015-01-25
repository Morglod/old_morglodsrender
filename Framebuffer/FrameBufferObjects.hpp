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

    bool AttachTextureMipmap(class ITexture* tex, Attachment const& attachment, unsigned int const& mipmapLevel) override;
    bool AttachTextureMipmapToColor(class ITexture* tex, unsigned int const& colorSlot, unsigned int const& mipmapLevel) override;

    virtual ~FrameBuffer();
protected:
    BindTarget _bindedTarget;
};

IFrameBuffer* FrameBufferGetBinded(IFrameBuffer::BindTarget const& target);
void FrameBufferUnBind(IFrameBuffer::BindTarget const& target);

}
