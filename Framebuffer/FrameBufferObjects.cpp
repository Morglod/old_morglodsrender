#include "FrameBufferObjects.hpp"
#include "FrameBufferConfig.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Containers.hpp"
#include "../Textures/TextureInterfaces.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

#define _MR_FB_BIND_TARGETS_NUM_ 3
mr::TStaticArray<mr::IFrameBuffer*> _MR_FB_BIND_TARGETS_(new mr::IFrameBuffer*[_MR_FB_BIND_TARGETS_NUM_], _MR_FB_BIND_TARGETS_NUM_, true);

class _MR_FB_BIND_TARGETS_NULL_ {
public:
    _MR_FB_BIND_TARGETS_NULL_() {
        for(size_t i = 0; i < _MR_FB_BIND_TARGETS_NUM_; ++i){
            _MR_FB_BIND_TARGETS_.GetRaw()[i] = nullptr;
        }
    }
};
_MR_FB_BIND_TARGETS_NULL_ _MR_FB_BIND_TARGETS_NULL_DEF_;

size_t _MR_FB_BIND_TARGETS_REMAP_FROM_INDEX_[] {
    GL_DRAW_FRAMEBUFFER,
    GL_READ_FRAMEBUFFER,
    GL_FRAMEBUFFER
};

namespace mr {

bool FrameBuffer::Create() {
    if(GetGPUHandle() != 0) {
        Destroy();
    }

    if(mr::gl::IsOpenGL45())
        glCreateFramebuffers(1, &_handle);
    else
        glGenFramebuffers(1, &_handle);

    return true;
}

void FrameBuffer::Bind(BindTarget const& target) {
    if(target == NotBinded) {
        _bindedTarget = target;
        return;
    }

    Assert(target < 0)
    Assert((size_t)target >= _MR_FB_BIND_TARGETS_NUM_)
    Assert(GetGPUHandle() == 0)

    if(_MR_FB_BIND_TARGETS_.GetRaw()[(size_t)target] == dynamic_cast<mr::IFrameBuffer*>(this)) return;
    _MR_FB_BIND_TARGETS_.GetRaw()[(size_t)target] = dynamic_cast<mr::IFrameBuffer*>(this);
    _bindedTarget = target;

    MR_FRAMEBUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindFramebuffer(_MR_FB_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], GetGPUHandle());
    )
}

IFrameBuffer* FrameBuffer::ReBind(BindTarget const& target) {
    if(target == NotBinded) {
        _bindedTarget = target;
        return 0;
    }

    Assert(target < 0)
    Assert((size_t)target >= _MR_FB_BIND_TARGETS_NUM_)

    IFrameBuffer* binded = _MR_FB_BIND_TARGETS_.GetRaw()[(size_t)target];
    Bind(target);
    return binded;
}

IFrameBuffer::CompletionStatus FrameBuffer::CheckCompletion(BindTarget const& target) {
    if(mr::gl::IsDirectStateAccessSupported()) {
        return (CompletionStatus)glCheckNamedFramebufferStatusEXT(GetGPUHandle(), _MR_FB_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target]);
    }
    else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) {
            binded = mr::FrameBuffer::ReBind(target);
        }
        auto result = glCheckFramebufferStatus(_MR_FB_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target]);
        if(binded) binded->Bind(target);
        return (CompletionStatus)result;
    }
}

bool FrameBuffer::AttachTextureMipmap(class ITexture* tex, Attachment const& attachment, unsigned int const& mipmapLevel) {
    Assert(tex == nullptr)

    if(mr::gl::IsDirectStateAccessSupported()) {
        glNamedFramebufferTexture(GetGPUHandle(), attachment, tex->GetGPUHandle(), mipmapLevel);
    } else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) binded = mr::FrameBuffer::ReBind(IFrameBuffer::DrawReadFramebuffer);
        glFramebufferTexture(IFrameBuffer::DrawReadFramebuffer, attachment, tex->GetGPUHandle(), mipmapLevel);
        if(binded) binded->Bind(IFrameBuffer::DrawReadFramebuffer);
    }
    return true;
}

bool FrameBuffer::AttachTextureMipmapToColor(class ITexture* tex, unsigned int const& colorSlot, unsigned int const& mipmapLevel) {
    Assert(tex == nullptr)

    if(mr::gl::IsDirectStateAccessSupported()) {
        glNamedFramebufferTexture(GetGPUHandle(), GL_COLOR_ATTACHMENT0 + colorSlot, tex->GetGPUHandle(), mipmapLevel);
    } else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) binded = mr::FrameBuffer::ReBind(IFrameBuffer::DrawReadFramebuffer);
        glFramebufferTexture(IFrameBuffer::DrawReadFramebuffer, GL_COLOR_ATTACHMENT0 + colorSlot, tex->GetGPUHandle(), mipmapLevel);
        if(binded) binded->Bind(IFrameBuffer::DrawReadFramebuffer);
    }
    return true;
}

void FrameBuffer::Destroy() {
    glDeleteFramebuffers(1, &_handle);
    _handle = 0;
    OnGPUHandleChanged(dynamic_cast<mr::IGPUObjectHandle*>(this), 0);
}

FrameBuffer::~FrameBuffer() {}

IFrameBuffer* FrameBufferGetBinded(IFrameBuffer::BindTarget const& target) {
    return _MR_FB_BIND_TARGETS_.GetRaw()[(size_t)target];
}

void FrameBufferUnBind(IFrameBuffer::BindTarget const& target) {
    auto ar = _MR_FB_BIND_TARGETS_.GetRaw();
    if(ar[target] == nullptr) return;
    ar[target] = nullptr;
    glBindFramebuffer(_MR_FB_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], 0);
}

}
