#pragma once

#include "Geometry/GeometryFormats.hpp"

#include <mu/Containers.hpp>
#include <vector>
#include <unordered_map>

namespace mr {

class IBuffer;

class Texture;
class TextureSettings;
class FrameBuffer;
class RenderBuffer;
class IShaderProgram;

struct GeomDataType;
struct VertexAttributeDesc;
struct VertexAttribute;
struct VertexFormat;
struct IndexFormat;

typedef std::shared_ptr<GeomDataType> GeomDataTypePtr;
typedef std::shared_ptr<VertexAttributeDesc> VertexAttributeDescPtr;
typedef std::shared_ptr<VertexAttribute> VertexAttributePtr;
typedef std::shared_ptr<VertexFormat> VertexFormatPtr;
typedef std::shared_ptr<IndexFormat> IndexFormatPtr;

class StateCache;
typedef std::shared_ptr<StateCache> StateCachePtr;
typedef std::weak_ptr<StateCache> StateCacheWeakPtr;

class StateCache {
public:
    enum BufferBindTarget : unsigned char {
        ArrayBuffer = 0,
        AtomicCounterBuffer = 1,
        CopyReadBuffer,
        CopyWriteBuffer,
        DrawIndirectBuffer,
        DispatchIndirectBuffer,
        ElementArrayBuffer,
        PixelPackBuffer,
        PixelUnpackBuffer,
        QueryBuffer,
        ShaderStorageBuffer,
        TextureBuffer,
        TransformFeedbackBuffer,
        UniformBuffer,

        NotBinded = 255
    };

    enum FrameBufferBindTarget : unsigned char {
        TargetFrameBuffer = 0,
        DrawFrameBuffer = 1,
        ReadFrameBuffer,

        FBO_TARGETS_NUM
    };

    void ResetCache();
    bool ReBindBuffers();
    bool ReBindUBOs();
    bool ReBindTransformFeedbacks();
	bool ReBindTextures();
	bool ReBindFrameBuffers();
    bool ReBindAll();

    bool BindBuffer(IBuffer* buffer, BufferBindTarget const& target);
    bool BindUniformBuffer(IBuffer* buffer, unsigned int const& index);
    bool BindTransformFeedbackBuffer(IBuffer* buffer, unsigned int const& index);
    IBuffer* GetBindedBuffer(BufferBindTarget const& target);
    IBuffer* GetBindedUniformBuffer(unsigned int const& index);
    IBuffer* GetBindedTransformFeedbackBuffer(unsigned int const& index);
    bool ReBindBuffer(IBuffer* __restrict__ buffer, BufferBindTarget const& target, IBuffer** __restrict__ was);

    void TextureUnitNotUsed(unsigned int const& unit);
    bool BindTexture(Texture* texture, unsigned int const& unit);
    Texture* GetBindedTexture(unsigned int const& unit);
    TextureSettings* GetBindedTextureSettings(unsigned int const& unit);
    bool ReBindTexture(Texture* __restrict__ texture, unsigned int const& unit, Texture** __restrict__ was);
    bool GetFreeTextureUnit(unsigned int& outFreeUnit);
    bool IsTextureUnitFree(unsigned int const& unit);

    bool BindTextureNotCached(unsigned int const& unit, unsigned int const& gpu_handle, unsigned int const& tex_type);
    bool GetBindedTextureNotCached(unsigned int const& unit, unsigned int& out_GPUHandle, unsigned int& out_TexType);

	bool BindFramebuffer(FrameBuffer* frameBuffer, FrameBufferBindTarget const& target);
	FrameBuffer* GetBindedFramebuffer(FrameBufferBindTarget const& target);
	bool ReBindFramebuffer(FrameBuffer* __restrict__ frameBuffer, FrameBufferBindTarget const& target, FrameBuffer** __restrict__ was);

    bool DrawTo(FrameBuffer* target);

	bool BindRenderbuffer(RenderBuffer* renderBuffer);
	RenderBuffer* GetBindedRenderbuffer();
	bool ReBindRenderbuffer(RenderBuffer* __restrict__ renderBuffer, RenderBuffer** __restrict__ was);

    bool SetShaderProgram(IShaderProgram* shaderProgram);
    bool ReSetShaderProgram(IShaderProgram* __restrict__ shaderProgram, IShaderProgram** __restrict__ was);
    IShaderProgram* GetShaderProgram();

    bool SetVertexFormat(VertexFormatPtr const& format);
    VertexFormatPtr GetVertexFormat();

    bool SetIndexFormat(IndexFormatPtr const& format);
    IndexFormatPtr GetIndexFormat();

    bool BindVertexAttribute(VertexAttribute const& attribute, unsigned int const& vertexSize);
    inline VertexAttribute* GetVertexAttribute(unsigned int const& index) {
        return &(_vertexAttributes.GetArray()[index]);
    }
    void UnBindVertexAttribute(unsigned int const& index);

    bool SetVertexBuffer(IBuffer* buf);
    bool SetVertexBuffer(IBuffer* buf, VertexFormatPtr const& format);
    IBuffer* GetVertexBuffer();

    bool SetIndexBuffer(IBuffer* buf);
    bool SetIndexBuffer(IBuffer* buf, IndexFormatPtr const& format);
    IBuffer* GetIndexBuffer();

    virtual ~StateCache();

    static StateCache* GetDefault(); //this thread default, 0 index
    static StateCacheWeakPtr New(size_t& outThisThreadIndex);
    static mu::ArrayRef<StateCacheWeakPtr> GetAll();
    static mu::ArrayHandle<StateCacheWeakPtr> GetThisThread();
    static StateCacheWeakPtr GetThisThread(size_t const& index);
private:
    StateCache();
    void _Init();

    mu::ArrayHandle<IBuffer*> _buffers;
    std::unordered_map<unsigned int, IBuffer*> _transformFeedbacks;
    mu::ArrayHandle<Texture*> _textures;
    mu::ArrayHandle<TextureSettings*> _textureSettings;
    mu::ArrayHandle<FrameBuffer*> _fbs;
    FrameBuffer* _drawTo;
    RenderBuffer* _renderBuffer;
    IShaderProgram* _shaderProgram;
    VertexFormatPtr _vertexFormat;
    IndexFormatPtr _indexFormat;

    mu::ArrayHandle<VertexAttribute> _vertexAttributes; // attribute[shader index]
    mu::ArrayHandle<IBuffer*> _ubos;
    unsigned int _vertexSize;
};

}
