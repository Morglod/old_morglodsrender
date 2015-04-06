#pragma once

#include "../Utils/Memory.hpp"
#include "../CoreObjects.hpp"

#include <Containers.hpp>

#include <glm/glm.hpp>

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

    enum AttachmentTarget {
        ColorTarget = 0x00004000,
        DepthTarget = 0x00000100,
        StencilTarget = 0x00000400
    };

    enum TargetFilter {
        Nearest = 0x2600,
        Linear = 0x2601
    };

    enum TargetBuffer {
        NoneTarget = 0,
        FrontLeftTarget = 0x0400,
        FrontRightTarget = 0x0401,
        BackLeftTarget = 0x0402,
        BackRigthTarget = 0x0403,
        FrontTarget = 0x0404,
        BackTarget = 0x0405,
        LeftTarget = 0x0406,
        RigthTarget = 0x0407,
        FrontAndBackTarget = 0x0408,
        DefaultTarget = BackTarget
    };

    static std::string CompletionStatusToString(CompletionStatus const& cs);

    virtual bool Create() = 0;
    virtual bool Bind(BindTarget const& target) = 0;
    virtual IFrameBuffer* ReBind(BindTarget const& target) = 0;
    virtual BindTarget GetBindTarget() = 0;

    virtual CompletionStatus CheckCompletion(BindTarget const& target) = 0;

    virtual bool SetTextureMipmap(ITexture* tex, Attachment const& attachment, unsigned int const& mipmapLevel) = 0;
    virtual bool SetTextureMipmapToColor(ITexture* tex, unsigned int const& colorSlot, unsigned int const& mipmapLevel) = 0;
    virtual bool SetRenderBuffer(IRenderBuffer* renderBuffer, Attachment const& attachment) = 0;
    virtual bool SetRenderBufferToColor(IRenderBuffer* renderBuffer, unsigned int const& colorSlot) = 0;

    virtual bool DrawToTarget(TargetBuffer const& targetBuffer) = 0;
    virtual bool DrawToAttachment(Attachment const& attachment) = 0;
    virtual bool DrawToColor(unsigned int const& colorSlot) = 0;

    virtual bool DrawTo(mu::ArrayHandle<TargetBuffer> const& targetBuffers,
                        mu::ArrayHandle<Attachment> const& attachments,
                        mu::ArrayHandle<unsigned int> const& colorSlots) = 0;
    virtual bool DrawTo(mu::ArrayHandle<unsigned int> const& glDrawBuffers) = 0;

    virtual void ToTarget(TargetBuffer const& targetBuffer, glm::ivec4 const& srcRect, glm::ivec4 const& dstRect, AttachmentTarget const& attachmentTarget, TargetFilter const& filter) = 0;

    virtual ~IFrameBuffer() {}
};

}
