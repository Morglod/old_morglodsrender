#pragma once

#include "build.hpp"
#include "mr/texture.hpp"
#include "mr/pre/glm.hpp"

#include <memory>

namespace mr {

typedef std::shared_ptr<class Texture2D> Texture2DPtr;
typedef std::shared_ptr<class FrameBuffer> FrameBufferPtr;
typedef std::shared_ptr<class RenderBuffer> RenderBufferPtr;

enum class FrameBufferCompletion : uint32_t {
    Complete = 0x8CD5,
    IncompleteAttachment = 0x8CD6,
    IncompleteMissingAttachment = 0x8CD7,
    IncompleteDrawBuffer = 0x8CDB,
    IncompleteReadBuffer = 0x8CDC,
    Unsupported = 0x8CDD,
    Undefined = 0x8219,
    IncompleteMultisample = 0x8D56,
    IncompleteLayerTargets = 0x8DA8,
    IncompleteLayerCount = 0x8DA9,
    IncompleteFormats = 0x8CDA
};

enum FrameBufferBit : uint32_t {
    Color = 0x00004000,
    Depth = 0x00000100,
    Stencil = 0x00000400,
    ColorDepth = 0x00004000 | 0x00000100,
    ColorDepthStencil = 0x00004000 | 0x00000100 | 0x00000400,
    ColorStencil = 0x00004000 | 0x00000400,
    DepthStencil = 0x00000100 | 0x00000400,
    All = FrameBufferBit::ColorDepthStencil
};

class MR_API FrameBuffer final {
    friend class _Alloc;
public:
    static FrameBufferPtr Create();
    void Destroy();

    bool SetColor(Texture2DPtr const& texture, const uint16_t attachmentIndex, const int32_t textureTargetMipmapLevel = 0);
    bool SetDepth(Texture2DPtr const& texture, const int32_t textureTargetMipmapLevel = 0);
    bool SetStencil(Texture2DPtr const& texture, const int32_t textureTargetMipmapLevel = 0);

    bool SetColor(RenderBufferPtr const& renderbuffer, const uint16_t attachmentIndex);
    bool SetDepth(RenderBufferPtr const& renderbuffer);
    bool SetStencil(RenderBufferPtr const& renderbuffer);

    bool CheckCompletion(FrameBufferCompletion& out_status);

    bool IsColorAttached(const uint16_t attachmentIndex);
    bool IsDepthAttached();
    bool IsStencilAttached();

    bool ToScreen(FrameBufferBit const& bitflags, glm::lowp_uvec4 const& src, glm::lowp_uvec4 const& dst, bool linear = false);
    bool ToFrameBuffer(FrameBufferPtr const& other, FrameBufferBit const& bitflags, glm::lowp_uvec4 const& src, glm::lowp_uvec4 const& dst, bool linear = false);

    inline uint32_t GetId() const;

    ~FrameBuffer();

private:
    FrameBuffer();

    uint32_t _id;
};

struct RenderBufferParams {
    TextureStorageFormat format = TextureStorageFormat::RGB8;
    glm::uvec2 size;
    int32_t samples = 0; // multisample
};

class MR_API RenderBuffer final {
    friend class _Alloc;
public:
    static RenderBufferPtr Create(RenderBufferParams const& params);
    inline static std::vector<RenderBufferPtr> Create(glm::uvec2 const& size, std::vector<TextureStorageFormat> const& formats);

    void Destroy();

    inline uint32_t GetId() const;

    ~RenderBuffer();
private:
    RenderBuffer();

    uint32_t _id;
};

inline uint32_t FrameBuffer::GetId() const { return _id; }
inline uint32_t RenderBuffer::GetId() const { return _id; }

MR_API void SetDrawFramebuffer(FrameBufferPtr const& target = nullptr); // if nullptr, default is used
MR_API void SetReadFramebuffer(FrameBufferPtr const& target = nullptr); // if nullptr, default is used

inline std::vector<RenderBufferPtr> RenderBuffer::Create(glm::uvec2 const& size, std::vector<TextureStorageFormat> const& formats) {
    RenderBufferParams params;
    params.size = size;
    std::vector<RenderBufferPtr> result(formats.size());
    for(size_t i = 0; i < formats.size(); ++i) {
        params.format = formats[i];
        result[i] = RenderBuffer::Create(params);
    }
    return result;
}

}
