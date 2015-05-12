#pragma once

#include "FrameBufferInterfaces.hpp"

namespace mr {

class FrameBuffer : public IFrameBuffer {
public:
    bool Create() override;
    void Destroy() override;

    bool Bind(BindTarget const& target) override;
    IFrameBuffer* ReBind(BindTarget const& target) override;
    inline BindTarget GetBindTarget() override { return _bindedTarget; }

    CompletionStatus CheckCompletion(BindTarget const& target) override;

    bool SetTextureMipmap(class Texture* tex, Attachment const& attachment, unsigned int const& mipmapLevel) override;
    bool SetTextureMipmapToColor(class Texture* tex, unsigned int const& colorSlot, unsigned int const& mipmapLevel) override;
    bool SetRenderBuffer(class IRenderBuffer* renderBuffer, Attachment const& attachment) override;
    bool SetRenderBufferToColor(class IRenderBuffer* renderBuffer, unsigned int const& colorSlot) override;

    bool DrawToTarget(TargetBuffer const& targetBuffer) override;
    bool DrawToAttachment(Attachment const& attachment) override;
    bool DrawToColor(unsigned int const& colorSlot) override;

    bool DrawTo(mu::ArrayHandle<TargetBuffer> const& targetBuffers,
                mu::ArrayHandle<Attachment> const& attachments,
                mu::ArrayHandle<unsigned int> const& colorSlots) override;

    bool DrawTo(mu::ArrayHandle<unsigned int> const& glDrawBuffers) override;

    void ToTarget(TargetBuffer const& targetBuffer, glm::ivec4 const& srcRect, glm::ivec4 const& dstRect, AttachmentTarget const& attachmentTarget, TargetFilter const& filter)  override;

    FrameBuffer();
    virtual ~FrameBuffer();
protected:
    BindTarget _bindedTarget;
};

bool DrawTo(IFrameBuffer::TargetBuffer const& targetBuffer);

IFrameBuffer* FrameBufferGetBinded(IFrameBuffer::BindTarget const& target);
void FrameBufferUnBind(IFrameBuffer::BindTarget const& target);

}
