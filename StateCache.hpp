#pragma once

#include "Geometry/GeometryFormats.hpp"

#include <Containers.hpp>
#include <vector>
#include <unordered_map>

namespace mr {

class IGPUBuffer;

class ITexture;
class TextureSettings;
class IFrameBuffer;
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

    void ResetCache();
    bool ReBindBuffers();
    bool ReBindUBOs();
    bool ReBindTransformFeedbacks();
	bool ReBindTextures();
	bool ReBindFrameBuffers();
    bool ReBindAll();

    bool BindBuffer(IGPUBuffer* buffer, BufferBindTarget const& target);
    bool BindUniformBuffer(IGPUBuffer* buffer, unsigned int const& index);
    bool BindTransformFeedbackBuffer(IGPUBuffer* buffer, unsigned int const& index);
    IGPUBuffer* GetBindedBuffer(BufferBindTarget const& target);
    IGPUBuffer* GetBindedUniformBuffer(unsigned int const& index);
    IGPUBuffer* GetBindedTransformFeedbackBuffer(unsigned int const& index);
    bool ReBindBuffer(IGPUBuffer* __restrict__ buffer, BufferBindTarget const& target, IGPUBuffer** __restrict__ was);

    void TextureUnitNotUsed(unsigned int const& unit);
    bool BindTexture(ITexture* texture, unsigned int const& unit);
    ITexture* GetBindedTexture(unsigned int const& unit);
    TextureSettings* GetBindedTextureSettings(unsigned int const& unit);
    bool ReBindTexture(ITexture* __restrict__ texture, unsigned int const& unit, ITexture** __restrict__ was);
    bool GetFreeTextureUnit(unsigned int& outFreeUnit);
    bool IsTextureUnitFree(unsigned int const& unit);

	bool BindFramebuffer(IFrameBuffer* frameBuffer);
	IFrameBuffer* GetBindedFramebuffer();
	bool ReBindFramebuffer(IFrameBuffer* __restrict__ frameBuffer, IFrameBuffer** __restrict__ was);

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

    bool SetVertexBuffer(IGPUBuffer* buf);
    bool SetVertexBuffer(IGPUBuffer* buf, VertexFormatPtr const& format);
    IGPUBuffer* GetVertexBuffer();

    bool SetIndexBuffer(IGPUBuffer* buf);
    bool SetIndexBuffer(IGPUBuffer* buf, IndexFormatPtr const& format);
    IGPUBuffer* GetIndexBuffer();

    virtual ~StateCache();

    static StateCache* GetDefault(); //this thread default, 0 index
    static StateCacheWeakPtr New(size_t& outThisThreadIndex);
    static mu::ArrayRef<StateCacheWeakPtr> GetAll();
    static mu::ArrayHandle<StateCacheWeakPtr> GetThisThread();
    static StateCacheWeakPtr GetThisThread(size_t const& index);
private:
    StateCache();
    void _Init();

    mu::ArrayHandle<IGPUBuffer*> _buffers;
    std::unordered_map<unsigned int, IGPUBuffer*> _ubos;
    std::unordered_map<unsigned int, IGPUBuffer*> _transformFeedbacks;
    mu::ArrayHandle<ITexture*> _textures;
    mu::ArrayHandle<TextureSettings*> _textureSettings;
    IFrameBuffer* _framebuffer;
    IShaderProgram* _shaderProgram;
    VertexFormatPtr _vertexFormat;
    IndexFormatPtr _indexFormat;

    mu::ArrayHandle<VertexAttribute> _vertexAttributes; // attribute[shader index]
    unsigned int _vertexSize;
};

}
