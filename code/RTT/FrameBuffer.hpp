#pragma once

#include "../CoreObjects.hpp"
#include <mu/Containers.hpp>

#include <unordered_map>
#include <glm/glm.hpp>

namespace mr {

class Texture;
class RenderBuffer;

class FrameBuffer : public IGPUObjectHandle {
    friend class RTTManager;
public:
    enum CompletionInfo : unsigned int {
        Success = 0,
        IncompleteAttachment = 1,
        IncompleteMissingAttachment,
        IncompleteDrawBuffer,
        IncompleteReadBuffer,
        Unsupported,
        Undefined,
        IncompleteMultisample,
        IncompleteLayerTargets,
        IncompleteLayerCount,
        IncompleteFormats,
        Failed
    };

    enum ToScreenFlags : unsigned int {
        ColorBit = 0x00004000,
        DepthBit = 0x00000100,
        StencilBit = 0x00000400
    };

    void Destroy() override;

    virtual bool AttachColor(Texture* tex, unsigned short const& colorAttachmentIndex);
    virtual bool AttachDepth(Texture* tex);
    virtual bool AttachStencil(Texture* tex);

    virtual bool AttachColor(RenderBuffer* rb, unsigned short const& colorAttachmentIndex);
    virtual bool AttachDepth(RenderBuffer* tex);
    virtual bool AttachStencil(RenderBuffer* tex);

    virtual CompletionInfo Complete();

    virtual bool IsColorAttached(unsigned short const& colorAttachmentIndex);
    virtual bool IsDepthAttached();
    virtual bool IsStencilAttached();

    virtual bool ToScreen(ToScreenFlags const& bitflags, glm::lowp_uvec4 const& src, glm::lowp_uvec4 const& dst, bool linear = false);

    inline virtual mu::ArrayRef<unsigned int> GetTargets() { return mu::ArrayRef<unsigned int>(_targets.GetArray(), _targets.GetNum()); }

    static std::string CompletionInfoToString(CompletionInfo const& info);

    virtual ~FrameBuffer();
protected:
    virtual bool Create();
    virtual void _ResetTargetsArray();

    FrameBuffer() = default;
    std::unordered_map<unsigned int, bool> _targets_map;
    mu::ArrayHandle<unsigned int> _targets;
};

}
