#include "StateCache.hpp"
#include "Utils/Log.hpp"
#include "Utils/Debug.hpp"
#include "MachineInfo.hpp"
#include "RTT/FrameBufferInterfaces.hpp"
#include "Buffers/BuffersInterfaces.hpp"
#include "Textures/TextureInterfaces.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

//without transform and uniform
#define MR_BUFFERS_BIND_TARGETS_NUM 12

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

namespace mr {

std::vector<StateCacheWeakPtr> MR_STATE_CACHES;
thread_local std::vector<StateCachePtr> MR_STATE_CACHES_THIS_THREAD = std::vector<StateCachePtr>();

void StateCache::ResetCache() {
    //Buffers
    _buffers = mu::ArrayHandle<IGPUBuffer*>(new IGPUBuffer*[MR_BUFFERS_BIND_TARGETS_NUM], MR_BUFFERS_BIND_TARGETS_NUM, true);
    for(size_t i = 0; i < MR_BUFFERS_BIND_TARGETS_NUM; ++i) _buffers.GetArray()[i] = nullptr;

    //Textures
    int textureUnits = mr::gl::GetMaxTextureUnits();
    AssertAndExec(textureUnits >= 4, mr::Log::LogString("MaxTextureUnits less than 4. May be problems.", MR_LOG_LEVEL_WARNING));

    if(textureUnits == 0 || textureUnits == -1) {
        mr::Log::LogString("Cannot get MaxTextureUnits. May be serious problems. 4 will be used.", MR_LOG_LEVEL_ERROR);
        textureUnits = 4;
    }

    _textures = mu::ArrayHandle<mr::ITexture*>(new mr::ITexture*[textureUnits], textureUnits, true);
    _textureSettings = mu::ArrayHandle<mr::ITextureSettings*>(new mr::ITextureSettings*[textureUnits], textureUnits, true);

    ITexture** texturesArray = _textures.GetArray();
    ITextureSettings** textureSettingsArray = _textureSettings.GetArray();

    for(int i = 0; i < textureUnits; ++i){
        texturesArray[i] = nullptr;
        textureSettingsArray[i] = nullptr;
    }

    //Clear
    _ubos.clear();
    _transformFeedbacks.clear();
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
    if(_ubos.size() == 0) return true;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        for(auto const& ubo_pair : _ubos) {
            glBindBufferBase(GL_UNIFORM_BUFFER, ubo_pair.first, (ubo_pair.second == nullptr) ? 0 : ubo_pair.second->GetGPUHandle());
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
    ITexture** texArray = _textures.GetArray();
    ITextureSettings** texSetArray = _textureSettings.GetArray();

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
	return BindFramebuffer(_framebuffer);
}

bool StateCache::ReBindAll() {
    return 	ReBindBuffers() &&
			ReBindUBOs() &&
			ReBindTransformFeedbacks() &&
			ReBindTextures() &&
			ReBindFrameBuffers();
}

bool StateCache::BindBuffer(IGPUBuffer* buffer, BufferBindTarget const& target) {
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

bool StateCache::BindUniformBuffer(IGPUBuffer* buffer, unsigned int const& index) {
    if(buffer == nullptr || buffer == 0 || buffer->GetGPUHandle() == 0) {
        _ubos[index] = nullptr;
        MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
            glBindBufferBase(GL_UNIFORM_BUFFER, index, 0); ,
            return false;
        )
        return true;
    }

    if(_ubos[index] == buffer) return true;

    _ubos[index] = buffer;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer->GetGPUHandle()); ,
        return false;
    )
    return true;
}

bool StateCache::BindTransformFeedbackBuffer(IGPUBuffer* buffer, unsigned int const& index) {
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

IGPUBuffer* StateCache::GetBindedBuffer(BufferBindTarget const& target) {
    AssertAndExec((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM, return nullptr);
    return _buffers.GetArray()[(size_t)target];
}

IGPUBuffer* StateCache::GetBindedUniformBuffer(unsigned int const& index) {
    if(_ubos.count(index) == 0) return nullptr;
    return _ubos[index];
}

IGPUBuffer* StateCache::GetBindedTransformFeedbackBuffer(unsigned int const& index) {
    if(_transformFeedbacks.count(index) == 0) return nullptr;
    return _transformFeedbacks[index];
}

bool StateCache::ReBindBuffer(IGPUBuffer* buffer, BufferBindTarget const& target, IGPUBuffer** was) {
    IGPUBuffer* binded = GetBindedBuffer(target);
    if(binded == buffer) return true;
    *was = binded;
    if(BindBuffer(buffer, target)) return true;
    else {
        BindBuffer(*was, target);
        return false;
    }
}

bool StateCache::BindTexture(ITexture* texture, unsigned int const& unit) {
    AssertAndExec(unit < _textures.GetNum(), return false);

    if(_textures.GetArray()[unit] == texture) return true;

    unsigned int handle = ((texture != nullptr) ? texture->GetGPUHandle() : 0);
    unsigned int texType = MR_TEXTURE_TARGET[((texture != nullptr) ? texture->GetType() : ITexture::Base2D)];

    if(mr::gl::IsOpenGL45()) {
        glBindTextureUnit(unit, handle);
    }
    else if(mr::gl::IsDirectStateAccessSupported()) {
        glBindMultiTextureEXT(GL_TEXTURE0+unit, texType, handle);
    } else {
        int actived_tex = 0;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &actived_tex);
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(texType, handle);
        glActiveTexture(actived_tex);
    }

    mr::ITextureSettings* ts = (texture != nullptr) ? texture->GetSettings() : nullptr;
    _textures.GetArray()[unit] = texture;
    _textureSettings.GetArray()[unit] = ts;
    glBindSampler(unit, (ts) ? ts->GetGPUHandle() : 0);

    return true;
}

ITexture* StateCache::GetBindedTexture(unsigned int const& unit) {
    if(unit >= _textures.GetNum()) {
        mr::Log::LogString("Trying to get " + std::to_string(unit) + " texture unit. Bigger than max ("+ std::to_string(_textures.GetNum())+") texture unit. StateCache::GetBindedTexture.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
    return _textures.GetArray()[unit];
}

ITextureSettings* StateCache::GetBindedTextureSettings(unsigned int const& unit) {
    if(unit >= _textureSettings.GetNum()) {
        mr::Log::LogString("Trying to get " + std::to_string(unit) + " texture unit. Bigger than max ("+ std::to_string(_textureSettings.GetNum())+") texture unit. StateCache::GetBindedTextureSettings.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
    return _textureSettings.GetArray()[unit];
}

bool StateCache::ReBindTexture(ITexture* texture, unsigned int const& unit, ITexture** was) {
    if(unit >= _textures.GetNum()) {
        mr::Log::LogString("Trying to get " + std::to_string(unit) + " texture unit. Bigger than max ("+ std::to_string(_textures.GetNum())+") texture unit. StateCache::ReBindTexture.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    ITexture* binded = _textures.GetArray()[unit];
    if(binded == texture) return true;
    *was = binded;
    if(BindTexture(texture, unit)) return true;
    else {
        BindTexture(binded, unit);
        return false;
    }
}

bool StateCache::GetFreeTextureUnit(unsigned int& freeUnit) {
    ITexture** texArray = _textures.GetArray();
    for(unsigned int i = 0; i < _textures.GetNum(); ++i) {
        if(texArray == nullptr) {
            freeUnit = i;
            return true;
        }
    }
    return false;
}

bool StateCache::BindFramebuffer(IFrameBuffer* frameBuffer) {
	if(frameBuffer == _framebuffer) return true;
	//TODO error catch.
	glBindFramebuffer(GL_FRAMEBUFFER, (frameBuffer) ? frameBuffer->GetGPUHandle() : 0);
	return true;
}

IFrameBuffer* StateCache::GetBindedFramebuffer() {
	return _framebuffer;
}

bool StateCache::ReBindFramebuffer(IFrameBuffer* frameBuffer, IFrameBuffer** was) {
	IFrameBuffer* binded = GetBindedFramebuffer();
	if(binded == frameBuffer) return true;
	if(!BindFramebuffer(frameBuffer)) {
		BindFramebuffer(binded);
		return false;
	}
	if(binded) *was = binded;
	return true;
}


StateCache::StateCache() {
}

StateCache::~StateCache() {
}

StateCacheWeakPtr StateCache::New() {
    StateCachePtr stateCache(new StateCache());
    stateCache->ResetCache();
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
    if(MR_STATE_CACHES_THIS_THREAD.empty()) New();
    return MR_STATE_CACHES_THIS_THREAD[0].get();
}

}
