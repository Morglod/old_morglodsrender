#include "StateCache.hpp"
#include "Utils/Log.hpp"
#include "Utils/Debug.hpp"
#include "MachineInfo.hpp"
#include "RTT/FrameBuffer.hpp"
#include "RTT/RenderBuffer.hpp"
#include "Buffers/BuffersInterfaces.hpp"
#include "Textures/Texture.hpp"
#include "Shaders/ShaderProgramObject.hpp"
#include "Geometry/GeometryInterfaces.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

//without transform and uniform
#define MR_BUFFERS_BIND_TARGETS_NUM 12

namespace {

unsigned int MR_BUFFER_TARGET[] {
    GL_ARRAY_BUFFER,
    GL_ATOMIC_COUNTER_BUFFER,
    GL_COPY_READ_BUFFER,
    GL_COPY_WRITE_BUFFER,
    GL_DRAW_INDIRECT_BUFFER,
    GL_DISPATCH_INDIRECT_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER,
    GL_QUERY_BUFFER,
    GL_SHADER_STORAGE_BUFFER,
    GL_TEXTURE_BUFFER
};

unsigned int MR_TEXTURE_TARGET[]{
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D
};

unsigned int MR_FRAMEBUFFER_TARGET[mr::StateCache::FBO_TARGETS_NUM] {
    GL_FRAMEBUFFER,
    GL_DRAW_FRAMEBUFFER,
    GL_READ_FRAMEBUFFER
};

}

namespace mr {

namespace {
    std::vector<StateCacheWeakPtr> MR_STATE_CACHES;
    thread_local std::vector<StateCachePtr> MR_STATE_CACHES_THIS_THREAD = std::vector<StateCachePtr>();
}

void StateCache::ResetCache() {
    //Clear
    _transformFeedbacks.clear();

    //Buffers
    _buffers = mu::ArrayHandle<IBuffer*>(new IBuffer*[MR_BUFFERS_BIND_TARGETS_NUM], MR_BUFFERS_BIND_TARGETS_NUM, true);
    for(size_t i = 0; i < MR_BUFFERS_BIND_TARGETS_NUM; ++i) _buffers.GetArray()[i] = nullptr;

    //Textures
    int textureUnits = mr::gl::GetMaxTextureUnits();
    AssertAndExec(textureUnits >= 4, mr::Log::LogString("MaxTextureUnits less than 4. May be problems.", MR_LOG_LEVEL_WARNING));

    if(textureUnits == 0 || textureUnits == -1) {
        mr::Log::LogString("Cannot get MaxTextureUnits. May be serious problems. 4 will be used.", MR_LOG_LEVEL_ERROR);
        textureUnits = 4;
    }

    _textures = mu::ArrayHandle<mr::Texture*>(new mr::Texture*[textureUnits], textureUnits, true);
    _textureSettings = mu::ArrayHandle<mr::TextureSettings*>(new mr::TextureSettings*[textureUnits], textureUnits, true);

    Texture** texturesArray = _textures.GetArray();
    TextureSettings** textureSettingsArray = _textureSettings.GetArray();

    for(int i = 0; i < textureUnits; ++i){
        texturesArray[i] = nullptr;
        textureSettingsArray[i] = nullptr;
    }

    //VertexFormat
    size_t maxVertexAttribs = mr::gl::GetMaxVertexAttribsNum();
    _vertexAttributes = mu::ArrayHandle<VertexAttribute>(new VertexAttribute[maxVertexAttribs], maxVertexAttribs, true, false);

    //Ubos
    size_t maxUBOs = 16;
    _ubos = mu::ArrayHandle<IBuffer*>(new IBuffer*[maxUBOs], maxUBOs, true, false);
    for(size_t i = 0; i < maxUBOs; ++i){
        _ubos.GetArray()[i] = nullptr;
    }

    //FBOs
    const size_t fbosNum = StateCache::FBO_TARGETS_NUM;
    _fbs = mu::ArrayHandle<FrameBuffer*>(new FrameBuffer*[fbosNum], fbosNum, true, false);
    for(size_t i = 0; i < fbosNum; ++i){
        _fbs.GetArray()[i] = nullptr;
    }
}

bool StateCache::ReBindBuffers() {
    const auto bufAr = _buffers.GetArray();
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        for(size_t i = 0; i < MR_BUFFERS_BIND_TARGETS_NUM; ++i) {
            glBindBuffer(MR_BUFFER_TARGET[(size_t)i], (bufAr[i] == nullptr) ? 0 : bufAr[i]->GetGPUHandle());
        },
        return false;
    )
    return true;
}

bool StateCache::ReBindUBOs() {
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        IBuffer** ubos = _ubos.GetArray();
        for(size_t i = 0; i < _ubos.GetNum(); ++i) {
            glBindBufferBase(GL_UNIFORM_BUFFER, i, (ubos[i] == nullptr) ? 0 : ubos[i]->GetGPUHandle());
        },
        return false;
    )
    return true;
}

bool StateCache::ReBindTransformFeedbacks() {
    if(_transformFeedbacks.size() == 0) return true;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        for(auto const& tf_pair : _transformFeedbacks) {
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, tf_pair.first, (tf_pair.second == nullptr) ? 0 : tf_pair.second->GetGPUHandle());
        },
        return false;
    )
    return true;
}

bool StateCache::ReBindTextures() {
	const size_t num = _textures.GetNum();
    Texture** texArray = _textures.GetArray();
    TextureSettings** texSetArray = _textureSettings.GetArray();

	if(num == 0) return true;

	unsigned int handles[num + num]; // textureHandles[num] + samplerHandles[num]

	for(size_t i = 0; i < _textures.GetNum(); ++i) {
		handles[i] = (texArray[i]) ? texArray[i]->GetGPUHandle() : 0;
		handles[i + num] = (texSetArray[i]) ? texSetArray[i]->GetGPUHandle() : 0;
	}

	//TODO error catch.

    if(GLEW_ARB_multi_bind) {
        glBindTextures(0, num, handles);
        glBindSamplers(0, num, &(handles[num]));
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        for(unsigned int i = 0; i < num; ++i) {
            if(!stateCache->BindTexture(texArray[i], i)) {
                mr::Log::LogString("Bind texture failed in StateCache::ReBindTextures.", MR_LOG_LEVEL_ERROR);
            }
        }
    }

	return true;
}

bool StateCache::ReBindFrameBuffers() {
    //TODO
    return false;
	//return BindFramebuffer(_framebuffer);
}

bool StateCache::ReBindAll() {
    return 	ReBindBuffers() &&
			ReBindUBOs() &&
			ReBindTransformFeedbacks() &&
			ReBindTextures() &&
			ReBindFrameBuffers();
}

bool StateCache::BindBuffer(IBuffer* buffer, BufferBindTarget const& target) {
    /*if(target == IGPUBuffer::NotBinded) {
        mr::Log::LogString("Strange \"NotBinded\" target in StateCache::BindBuffer.", MR_LOG_LEVEL_WARNING);
        return false;
    }*/

    AssertAndExec((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM, return false);

    auto buffersArray = _buffers.GetArray();
    if(buffer == nullptr || buffer == 0 || buffer->GetGPUHandle() == 0) {
        buffersArray[(size_t)target] = nullptr;

        MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
            glBindBuffer(MR_BUFFER_TARGET[(size_t)target], 0); ,
            return false;
        )
        return true;
    }

    if(buffersArray[(size_t)target] == buffer) return true;

    buffersArray[(size_t)target] = buffer;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBuffer(MR_BUFFER_TARGET[(size_t)target], buffer->GetGPUHandle()); ,
        return false;
    )
    return true;
}

bool StateCache::BindUniformBuffer(IBuffer* buffer, unsigned int const& index) {
    IBuffer** ubos = _ubos.GetArray();
    if(buffer == nullptr || buffer == 0 || buffer->GetGPUHandle() == 0) {
        ubos[index] = nullptr;
        MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
            glBindBufferBase(GL_UNIFORM_BUFFER, index, 0); ,
            return false;
        )
        return true;
    }

    if(ubos[index] == buffer) return true;

    ubos[index] = buffer;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer->GetGPUHandle()); ,
        return false;
    )
    return true;
}

bool StateCache::BindTransformFeedbackBuffer(IBuffer* buffer, unsigned int const& index) {
    if(buffer == nullptr || buffer == 0 || buffer->GetGPUHandle() == 0) {
        _transformFeedbacks[index] = nullptr;
        MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, 0); ,
            return false;
        )
        return true;
    }

    if(_transformFeedbacks[index] == buffer) return true;

    _transformFeedbacks[index] = buffer;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer->GetGPUHandle()); ,
        return false;
    )
    return true;
}

IBuffer* StateCache::GetBindedBuffer(BufferBindTarget const& target) {
    AssertAndExec((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM, return nullptr);
    return _buffers.GetArray()[(size_t)target];
}

IBuffer* StateCache::GetBindedUniformBuffer(unsigned int const& index) {
    return _ubos.GetArray()[index];
}

IBuffer* StateCache::GetBindedTransformFeedbackBuffer(unsigned int const& index) {
    if(_transformFeedbacks.count(index) == 0) return nullptr;
    return _transformFeedbacks[index];
}

bool StateCache::ReBindBuffer(IBuffer* __restrict__ buffer, BufferBindTarget const& target, IBuffer** __restrict__ was) {
    IBuffer* binded = GetBindedBuffer(target);
    if(binded == buffer) return true;
    *was = binded;
    if(BindBuffer(buffer, target)) return true;
    else {
        BindBuffer(*was, target);
        return false;
    }
}

void StateCache::TextureUnitNotUsed(unsigned int const& unit) {
    AssertAndExec(unit < _textures.GetNum(), return);
    _textures.GetArray()[unit] = nullptr;
}

bool StateCache::BindTexture(Texture* texture, unsigned int const& unit) {
    AssertAndExec(unit < _textures.GetNum(), return false);

    if(texture != nullptr)
        if(_textures.GetArray()[unit] == texture) return true;

    unsigned int handle = ((texture != nullptr) ? texture->GetGPUHandle() : 0);
    unsigned int texType = MR_TEXTURE_TARGET[((texture != nullptr) ? texture->GetType() : Texture::Base2D)];

    if(mr::gl::IsOpenGL45()) {
        glBindTextureUnit(unit, handle);
    }
    else if(mr::gl::IsDSA_EXT()) {
        glBindMultiTextureEXT(GL_TEXTURE0+unit, texType, handle);
    } else {
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(texType, handle);
    }
    //TODO
    //mr::TextureSettings* ts = (texture != nullptr) ? texture->GetSettings() : nullptr;
    _textures.GetArray()[unit] = texture;
    //_textureSettings.GetArray()[unit] = ts;
    //glBindSampler(unit, (ts) ? ts->GetGPUHandle() : 0);

    return true;
}

Texture* StateCache::GetBindedTexture(unsigned int const& unit) {
    if(unit >= _textures.GetNum()) {
        mr::Log::LogString("Trying to get " + std::to_string(unit) + " texture unit. Bigger than max ("+ std::to_string(_textures.GetNum())+") texture unit. StateCache::GetBindedTexture.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
    return _textures.GetArray()[unit];
}

TextureSettings* StateCache::GetBindedTextureSettings(unsigned int const& unit) {
    if(unit >= _textureSettings.GetNum()) {
        mr::Log::LogString("Trying to get " + std::to_string(unit) + " texture unit. Bigger than max ("+ std::to_string(_textureSettings.GetNum())+") texture unit. StateCache::GetBindedTextureSettings.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
    return _textureSettings.GetArray()[unit];
}

bool StateCache::ReBindTexture(Texture* __restrict__ texture, unsigned int const& unit, Texture** __restrict__ was) {
    if(unit >= _textures.GetNum()) {
        mr::Log::LogString("Trying to get " + std::to_string(unit) + " texture unit. Bigger than max ("+ std::to_string(_textures.GetNum())+") texture unit. StateCache::ReBindTexture.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    Texture* binded = _textures.GetArray()[unit];
    if(binded == texture) return true;
    *was = binded;
    if(BindTexture(texture, unit)) return true;
    else {
        BindTexture(binded, unit);
        return false;
    }
}

bool StateCache::GetFreeTextureUnit(unsigned int& outFreeUnit) {
    Texture** texArray = _textures.GetArray();
    for(unsigned int i = 0; i < _textures.GetNum(); ++i) {
        if(texArray == nullptr) {
            outFreeUnit = i;
            return true;
        }
    }
    return false;
}

bool StateCache::IsTextureUnitFree(unsigned int const& unit) {
    if(_textures.GetNum() <= unit) return false;
    Texture** texArray = _textures.GetArray();
    return (texArray[unit] == nullptr);
}

bool StateCache::BindTextureNotCached(unsigned int const& unit, unsigned int const& gpu_handle, unsigned int const& tex_type) {
    if(_textures.GetNum() <= unit) {
        mr::Log::LogString("Failed StateCache::BindTextureNotCached. unit > maxTextureUnits.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(mr::gl::IsOpenGL45()) {
        glBindTextureUnit(unit, gpu_handle);
    }
    else if(mr::gl::IsDSA_EXT()) {
        glBindMultiTextureEXT(GL_TEXTURE0+unit, tex_type, gpu_handle);
    } else {
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(tex_type, gpu_handle);
    }
    return true;
}

bool StateCache::GetBindedTextureNotCached(unsigned int const& unit, unsigned int& out_GPUHandle, unsigned int& out_TexType) {
    if(_textures.GetNum() <= unit) {
        mr::Log::LogString("Failed StateCache::GetBindedTextureNotCached. unit > maxTextureUnits.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(_textures.GetArray()[unit] == nullptr) {
        out_GPUHandle = 0;
        out_TexType = 0;
    } else {
        out_GPUHandle = _textures.GetArray()[unit]->GetGPUHandle();
        out_TexType = _textures.GetArray()[unit]->GetType();
    }
    return true;
}

bool StateCache::BindFramebuffer(FrameBuffer* frameBuffer, FrameBufferBindTarget const& target) {
    FrameBuffer** fbos = _fbs.GetArray();
    const unsigned char index = (unsigned char)target;
    if(fbos[index] == frameBuffer) return true;

    const unsigned int handle = (frameBuffer == nullptr) ? 0 : frameBuffer->GetGPUHandle();
    glBindFramebuffer(MR_FRAMEBUFFER_TARGET[index], handle);
	fbos[index] = frameBuffer;

	return true;
}

FrameBuffer* StateCache::GetBindedFramebuffer(FrameBufferBindTarget const& target) {
    FrameBuffer** fbos = _fbs.GetArray();
    const unsigned char index = (unsigned char)target;
	return fbos[index];
}

bool StateCache::ReBindFramebuffer(FrameBuffer* __restrict__ frameBuffer, FrameBufferBindTarget const& target, FrameBuffer** __restrict__ was) {
	FrameBuffer* binded = GetBindedFramebuffer(target);
	if(binded == frameBuffer) return true;
	if(!BindFramebuffer(frameBuffer, target)) {
		BindFramebuffer(binded, target);
		return false;
	}
	if(binded) *was = binded;
	return true;
}

bool StateCache::DrawTo(FrameBuffer* target) {
    if(_drawTo == target) return true;
    if(target == nullptr) {
        glDrawBuffer(GL_BACK);
    } else {
        mu::ArrayRef<unsigned int> targets = target->GetTargets();
        glDrawBuffers(targets.num, targets.elements);
    }
    _drawTo = target;
    return BindFramebuffer(target, DrawFrameBuffer);
}

bool StateCache::BindRenderbuffer(RenderBuffer* renderBuffer) {
    if(_renderBuffer == renderBuffer) return true;
    const unsigned int handle = (renderBuffer == nullptr) ? 0 : renderBuffer->GetGPUHandle();
    glBindRenderbuffer(GL_RENDERBUFFER, handle);
    _renderBuffer = renderBuffer;
    return true;
}

RenderBuffer* StateCache::GetBindedRenderbuffer() {
    return _renderBuffer;
}

bool StateCache::ReBindRenderbuffer(RenderBuffer* __restrict__ renderBuffer, RenderBuffer** __restrict__ was) {
	RenderBuffer* binded = GetBindedRenderbuffer();
	if(binded == renderBuffer) return true;
	if(!BindRenderbuffer(renderBuffer)) {
		BindRenderbuffer(binded);
		return false;
	}
	if(binded) *was = binded;
	return true;
}

bool StateCache::SetShaderProgram(IShaderProgram* shaderProgram) {
    if(shaderProgram == _shaderProgram) {
        //_shaderProgram->UpdateUniforms();
        return true;
    }

    if(shaderProgram != nullptr) {
        if(!shaderProgram->IsLinked()) {
            mr::Log::LogString("Failed StateCache::BindShaderProgram. Can't use not linked shader program.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        const unsigned int handle = shaderProgram->GetGPUHandle();
        glUseProgram(handle);
        //shaderProgram->UpdateUniforms();
    } else {
        glUseProgram(0);
    }

    _shaderProgram = shaderProgram;
    return true;
}

bool StateCache::ReSetShaderProgram(IShaderProgram* __restrict__ shaderProgram, IShaderProgram** __restrict__ was) {
    IShaderProgram* binded = GetShaderProgram();
	if(binded == shaderProgram) return true;
	if(!SetShaderProgram(shaderProgram)) {
		SetShaderProgram(binded);
		return false;
	}
	if(binded) *was = binded;
	return true;
}

IShaderProgram* StateCache::GetShaderProgram() {
    return _shaderProgram;
}

bool StateCache::SetVertexFormat(VertexFormatPtr const& format) {
    if(format == _vertexFormat) return true;
    bool result = true;
    if(format == nullptr) {
        for(size_t i = 0; i < _vertexAttributes.GetNum(); ++i) {
            UnBindVertexAttribute(i);
        }
        if(mr::gl::IsNVVBUMSupported()){
            glDisableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
        }
    }
    if(format->Equal(_vertexFormat.get())) return true;
    //Bind
    for(size_t i = 0; i < format->attribsNum; ++i) {
        result = result && BindVertexAttribute(format->attributes.GetArray()[i], format->size);
    }
    //UnBind from last format if any
    if((_vertexFormat != nullptr) && (_vertexFormat->attribsNum > format->attribsNum)) {
        for(size_t i = 0; i < (_vertexFormat->attribsNum - format->attribsNum); ++i) {
            UnBindVertexAttribute(i+format->attribsNum);
        }
    }
    _vertexFormat = format;
    return result;
}

VertexFormatPtr StateCache::GetVertexFormat() {
    return _vertexFormat;
}

bool StateCache::SetIndexFormat(IndexFormatPtr const& format) {
    if(_indexFormat && _indexFormat->Equal(format.get())) return true;
    _indexFormat = format;

    if(mr::gl::IsNVVBUMSupported()){
        if(format)  glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
        else        glDisableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
    }
    return true;
}

IndexFormatPtr StateCache::GetIndexFormat() {
    return _indexFormat;
}

//TODO: assert for _vertexAttributes index

bool StateCache::BindVertexAttribute(VertexAttribute const& attribute, unsigned int const& vertexSize) {
    const unsigned int index = attribute.desc->shaderIndex;
    if(attribute.Equal(_vertexAttributes.GetArray()[index])) return true;
    if(mr::gl::IsNVVBUMSupported()) {
        glVertexAttribFormatNV(index, (int) attribute.desc->elementsNum, attribute.desc->dataType->dataTypeGL, GL_FALSE, vertexSize);
        glEnableVertexAttribArray(index);
        glVertexAttribDivisor(index, attribute.desc->divisor);
        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    }
    else {
        glVertexAttribPointer(index, (int) attribute.desc->elementsNum, attribute.desc->dataType->dataTypeGL, GL_FALSE, vertexSize, (const void*)((size_t)attribute.offset));
        glEnableVertexAttribArray(index);
        glVertexAttribDivisor(index, attribute.desc->divisor);
    }
    _vertexAttributes.GetArray()[index] = attribute;
    return true;
}

void StateCache::UnBindVertexAttribute(unsigned int const& index) {
    glDisableVertexAttribArray(index);
    _vertexAttributes.GetArray()[index] = VertexAttribute();
}

bool StateCache::SetVertexBuffer(IBuffer* buf) {
    if(!BindBuffer(buf, BufferBindTarget::ArrayBuffer)) return false;
    return true;
}

bool StateCache::SetVertexBuffer(IBuffer* buf, VertexFormatPtr const& format) {
    return SetVertexFormat(format) && SetVertexBuffer(buf);
}

IBuffer* StateCache::GetVertexBuffer() {
    return GetBindedBuffer(BufferBindTarget::ArrayBuffer);
}

bool StateCache::SetIndexBuffer(IBuffer* buf) {
    if(!BindBuffer(buf, BufferBindTarget::ElementArrayBuffer)) return false;
    return true;
}

bool StateCache::SetIndexBuffer(IBuffer* buf, IndexFormatPtr const& format) {
    return SetIndexFormat(format) && SetIndexBuffer(buf);
}

IBuffer* StateCache::GetIndexBuffer() {
    return GetBindedBuffer(BufferBindTarget::ElementArrayBuffer);
}

StateCache::StateCache() {
    _shaderProgram = nullptr;
    _vertexFormat = nullptr;
    _indexFormat = nullptr;
}

StateCache::~StateCache() {
}

StateCacheWeakPtr StateCache::New(size_t& outThisThreadIndex) {
    StateCachePtr stateCache(new StateCache());
    stateCache->ResetCache();
    outThisThreadIndex = MR_STATE_CACHES_THIS_THREAD.size();
    MR_STATE_CACHES_THIS_THREAD.push_back(stateCache);
    StateCacheWeakPtr weakPtr(stateCache);
    MR_STATE_CACHES.push_back(weakPtr);
    return weakPtr;
}

mu::ArrayRef<StateCacheWeakPtr> StateCache::GetAll() {
    return mu::ArrayRef<StateCacheWeakPtr>(MR_STATE_CACHES.data(), MR_STATE_CACHES.size());
}

mu::ArrayHandle<StateCacheWeakPtr> StateCache::GetThisThread() {
    mu::ArrayHandle<StateCacheWeakPtr> ar(new StateCacheWeakPtr[MR_STATE_CACHES_THIS_THREAD.size()], MR_STATE_CACHES_THIS_THREAD.size(), true);
    auto arPtr = ar.GetArray();
    for(size_t i = 0; i < MR_STATE_CACHES_THIS_THREAD.size(); ++i) arPtr[i] = StateCacheWeakPtr(MR_STATE_CACHES_THIS_THREAD[i]);
    return ar;
}

StateCacheWeakPtr StateCache::GetThisThread(size_t const& index) {
    if(index >= MR_STATE_CACHES_THIS_THREAD.size()) return StateCacheWeakPtr();
    return StateCacheWeakPtr(MR_STATE_CACHES_THIS_THREAD[index]);
}

StateCache* StateCache::GetDefault() {
    size_t indexNew = 0;
    if(MR_STATE_CACHES_THIS_THREAD.empty()) New(indexNew);
    return MR_STATE_CACHES_THIS_THREAD[indexNew].get();
}

}
