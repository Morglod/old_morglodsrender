#include "FrameBuffer.hpp"
#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"
#include "../Textures/Texture.hpp"
#include "RenderBuffer.hpp"
#include "../StateCache.hpp"

#include <GL\glew.h>

#include <unordered_map>

namespace {

std::unordered_map<unsigned int, unsigned char> MAP_GL_COMPLETE_STATUS_TO_INDEX = {
    { 0, 0 },
    { GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, 1 },
    { GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, 2 },
    { GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER, 3 },
    { GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER, 4 },
    { GL_FRAMEBUFFER_UNSUPPORTED, 5 },
    { GL_FRAMEBUFFER_UNDEFINED, 6 },
    { GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE, 7 },
    { GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS, 8, },
    { GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB, 9 },
    { GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT, 10 }
};

const std::string MAP_COMPLETE_STATUS_INDEX_TO_STRING[] = {
    std::string("Success"),
    std::string("IncompleteAttachment"),
    std::string("IncompleteMissingAttachment"),
    std::string("IncompleteDrawBuffer"),
    std::string("IncompleteReadBuffer"),
    std::string("Unsupported"),
    std::string("Undefined"),
    std::string("IncompleteMultisample"),
    std::string("IncompleteLayerTargets"),
    std::string("IncompleteLayerCount"),
    std::string("IncompleteFormats"),
    std::string("Failed")
};

}

namespace mr {

bool FrameBuffer::Create() {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        mr::Log::LogString("Failed FrameBuffer::Create. FrameBuffer already created.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    if(mr::gl::IsOpenGL45()) glCreateFramebuffers(1, &handle);
    else glGenFramebuffers(1, &handle);
    SetGPUHandle(handle);

    return true;
}

bool FrameBuffer::AttachColor(Texture* tex, unsigned short const& colorAttachmentIndex) {
    const unsigned int handle = GetGPUHandle();
    const unsigned int texHandle = (tex == nullptr) ? 0 : tex->GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed FrameBuffer::AttachColor. FrameBuffer is not created.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(mr::gl::IsDSA_ARB()) {
        glNamedFramebufferTexture(GetGPUHandle(), GL_COLOR_ATTACHMENT0+colorAttachmentIndex, texHandle, 0);
    } else if(mr::gl::IsDSA_EXT()) {
        glNamedFramebufferTextureEXT(GetGPUHandle(), GL_COLOR_ATTACHMENT0+colorAttachmentIndex, texHandle, 0);
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        FrameBuffer* was = stateCache->GetBindedFramebuffer(StateCache::TargetFrameBuffer);
        if(!stateCache->ReBindFramebuffer(this, StateCache::TargetFrameBuffer, &was)) {
            mr::Log::LogString("Failed FrameBuffer::AttachColor. Failed bind framebuffer.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, GL_TEXTURE_2D, texHandle, 0);
        if(was) stateCache->BindFramebuffer(was, StateCache::TargetFrameBuffer);
    }
    _targets_map[GL_COLOR_ATTACHMENT0+colorAttachmentIndex] = (bool)texHandle;

    return true;
}

bool FrameBuffer::AttachDepth(Texture* tex) {
    const unsigned int handle = GetGPUHandle();
    const unsigned int texHandle = (tex == nullptr) ? 0 : tex->GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed FrameBuffer::AttachDepth. FrameBuffer is not created.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(mr::gl::IsDSA_ARB()) {
        glNamedFramebufferTexture(GetGPUHandle(), GL_DEPTH_ATTACHMENT, texHandle, 0);
    } else if(mr::gl::IsDSA_EXT()) {
        glNamedFramebufferTextureEXT(GetGPUHandle(), GL_DEPTH_ATTACHMENT, texHandle, 0);
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        FrameBuffer* was = stateCache->GetBindedFramebuffer(StateCache::TargetFrameBuffer);
        if(!stateCache->ReBindFramebuffer(this, StateCache::TargetFrameBuffer, &was)) {
            mr::Log::LogString("Failed FrameBuffer::AttachDepth. Failed bind framebuffer.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texHandle, 0);
        if(was) stateCache->BindFramebuffer(was, StateCache::TargetFrameBuffer);
    }
    _targets_map[GL_DEPTH_ATTACHMENT] = (bool)texHandle;

    return true;
}

bool FrameBuffer::AttachStencil(Texture* tex) {
    const unsigned int handle = GetGPUHandle();
    const unsigned int texHandle = (tex == nullptr) ? 0 : tex->GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed FrameBuffer::AttachStencil. FrameBuffer is not created.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(mr::gl::IsDSA_ARB()) {
        glNamedFramebufferTexture(GetGPUHandle(), GL_STENCIL_ATTACHMENT, texHandle, 0);
    } else if(mr::gl::IsDSA_EXT()) {
        glNamedFramebufferTextureEXT(GetGPUHandle(), GL_STENCIL_ATTACHMENT, texHandle, 0);
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        FrameBuffer* was = stateCache->GetBindedFramebuffer(StateCache::TargetFrameBuffer);
        if(!stateCache->ReBindFramebuffer(this, StateCache::TargetFrameBuffer, &was)) {
            mr::Log::LogString("Failed FrameBuffer::AttachStencil. Failed bind framebuffer.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texHandle, 0);
        if(was) stateCache->BindFramebuffer(was, StateCache::TargetFrameBuffer);
    }
    _targets_map[GL_STENCIL_ATTACHMENT] = (bool)texHandle;

    return true;
}

bool FrameBuffer::AttachColor(RenderBuffer* rb, unsigned short const& colorAttachmentIndex) {
    const unsigned int handle = GetGPUHandle();
    const unsigned int rbHandle = (rb == nullptr) ? 0 : rb->GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed FrameBuffer::AttachColor. FrameBuffer is not created.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(mr::gl::IsDSA_ARB()) {
        glNamedFramebufferRenderbuffer(GetGPUHandle(), GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, GL_RENDERBUFFER, rbHandle);
    } else if(mr::gl::IsDSA_EXT()) {
        glNamedFramebufferRenderbufferEXT(GetGPUHandle(), GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, GL_RENDERBUFFER, rbHandle);
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        FrameBuffer* was = stateCache->GetBindedFramebuffer(StateCache::TargetFrameBuffer);
        if(!stateCache->ReBindFramebuffer(this, StateCache::TargetFrameBuffer, &was)) {
            mr::Log::LogString("Failed FrameBuffer::AttachColor. Failed bind renderbuffer.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, GL_RENDERBUFFER, rbHandle);
        if(was) stateCache->BindFramebuffer(was, StateCache::TargetFrameBuffer);
    }
    _targets_map[GL_COLOR_ATTACHMENT0 + colorAttachmentIndex] = (bool)rbHandle;

    return true;
}

bool FrameBuffer::AttachDepth(RenderBuffer* rb) {
    const unsigned int handle = GetGPUHandle();
    const unsigned int rbHandle = (rb == nullptr) ? 0 : rb->GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed FrameBuffer::AttachDepth. FrameBuffer is not created.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(mr::gl::IsDSA_ARB()) {
        glNamedFramebufferRenderbuffer(GetGPUHandle(), GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbHandle);
    } else if(mr::gl::IsDSA_EXT()) {
        glNamedFramebufferRenderbufferEXT(GetGPUHandle(), GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbHandle);
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        FrameBuffer* was = stateCache->GetBindedFramebuffer(StateCache::TargetFrameBuffer);
        if(!stateCache->ReBindFramebuffer(this, StateCache::TargetFrameBuffer, &was)) {
            mr::Log::LogString("Failed FrameBuffer::AttachDepth. Failed bind renderbuffer.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbHandle);
        if(was) stateCache->BindFramebuffer(was, StateCache::TargetFrameBuffer);
    }
    _targets_map[GL_DEPTH_ATTACHMENT] = (bool)rbHandle;

    return true;
}

bool FrameBuffer::AttachStencil(RenderBuffer* rb) {
    const unsigned int handle = GetGPUHandle();
    const unsigned int rbHandle = (rb == nullptr) ? 0 : rb->GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed FrameBuffer::AttachStencil. FrameBuffer is not created.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(mr::gl::IsDSA_ARB()) {
        glNamedFramebufferRenderbuffer(GetGPUHandle(), GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbHandle);
    } else if(mr::gl::IsDSA_EXT()) {
        glNamedFramebufferRenderbufferEXT(GetGPUHandle(), GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbHandle);
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        FrameBuffer* was = stateCache->GetBindedFramebuffer(StateCache::TargetFrameBuffer);
        if(!stateCache->ReBindFramebuffer(this, StateCache::TargetFrameBuffer, &was)) {
            mr::Log::LogString("Failed FrameBuffer::AttachStencil. Failed bind renderbuffer.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbHandle);
        if(was) stateCache->BindFramebuffer(was, StateCache::TargetFrameBuffer);
    }
    _targets_map[GL_STENCIL_ATTACHMENT] = (bool)rbHandle;

    return true;
}

FrameBuffer::CompletionInfo FrameBuffer::Complete() {
    unsigned int result = 0;
    if(mr::gl::IsDSA_ARB()) {
        result = glCheckNamedFramebufferStatus(GetGPUHandle(), GL_FRAMEBUFFER);
    } else if(mr::gl::IsDSA_EXT()) {
        result = glCheckNamedFramebufferStatusEXT(GetGPUHandle(), GL_FRAMEBUFFER);
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        FrameBuffer* was = stateCache->GetBindedFramebuffer(StateCache::TargetFrameBuffer);
        if(!stateCache->ReBindFramebuffer(this, StateCache::TargetFrameBuffer, &was)) {
            mr::Log::LogString("Failed FrameBuffer::CompletionStatus. Failed bind framebuffer.", MR_LOG_LEVEL_ERROR);
            return Failed;
        }
        result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(was) stateCache->BindFramebuffer(was, StateCache::TargetFrameBuffer);
    }

    _ResetTargetsArray();

    if(result == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB || result == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT) result = GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS;
    else if(result == GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB || result == GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT) result = GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB;
    else if(result == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT) result = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;
    else if(result == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT) result = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER;
    else if(result == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT) result = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER;
    else if(result == GL_FRAMEBUFFER_UNSUPPORTED_EXT) result = GL_FRAMEBUFFER_UNSUPPORTED;
    return (CompletionInfo) MAP_GL_COMPLETE_STATUS_TO_INDEX[result];
}

std::string FrameBuffer::CompletionInfoToString(CompletionInfo const& info) {
    return MAP_COMPLETE_STATUS_INDEX_TO_STRING[(size_t)info];
}

bool FrameBuffer::IsColorAttached(unsigned short const& colorAttachmentIndex) {
    auto it = _targets_map.find(GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);
    if(it == _targets_map.end()) return false;
    else return it->second;
}

bool FrameBuffer::IsDepthAttached() {
    auto it = _targets_map.find(GL_DEPTH_ATTACHMENT);
    if(it == _targets_map.end()) return false;
    else return it->second;
}

bool FrameBuffer::IsStencilAttached() {
    auto it = _targets_map.find(GL_STENCIL_ATTACHMENT);
    if(it == _targets_map.end()) return false;
    else return it->second;
}

void FrameBuffer::_ResetTargetsArray() {
    std::stack<unsigned int> elements;
    for(std::pair<unsigned int, bool> const& p : _targets_map) {
        if(p.second) elements.push(p.first);
    }
    if(elements.empty()) {
        _targets = mu::ArrayHandle<unsigned int>(nullptr, 0, true, false);
        return;
    }
    _targets = mu::ArrayHandle<unsigned int>(new unsigned int[elements.size()], elements.size(), true, false);
    unsigned int* targetsAr = _targets.GetArray();
    for(size_t i = 0; !elements.empty(); ++i) {
        targetsAr[i] = elements.top();
        elements.pop();
    }
}

bool FrameBuffer::ToScreen(ToScreenFlags const& bitflags, glm::lowp_uvec4 const& src, glm::lowp_uvec4 const& dst, bool linear) {
    if(mr::gl::IsDSA_ARB()) {
        glBlitNamedFramebuffer(GetGPUHandle(), 0, src.x, src.y, src.z, src.w, dst.x, dst.y, dst.z, dst.w, (unsigned int)bitflags, (linear) ? GL_LINEAR : GL_NEAREST);
    } else {
        FrameBuffer* wasRead = nullptr;
        FrameBuffer* wasDraw = nullptr;

        StateCache* stateCache = StateCache::GetDefault();
        if(!stateCache->ReBindFramebuffer(this, StateCache::ReadFrameBuffer, &wasRead)) {
            mr::Log::LogString("Failed FrameBuffer::ToScreen. Failed bind read framebuffer.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        if(!stateCache->ReBindFramebuffer(nullptr, StateCache::DrawFrameBuffer, &wasDraw)) {
            mr::Log::LogString("Failed FrameBuffer::ToScreen. Failed bind draw framebuffer.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        glBlitFramebuffer(src.x, src.y, src.z, src.w, dst.x, dst.y, dst.z, dst.w, (unsigned int)bitflags, (linear) ? GL_LINEAR : GL_NEAREST);

        if(wasRead) stateCache->BindFramebuffer(wasRead, StateCache::ReadFrameBuffer);
        if(wasDraw) stateCache->BindFramebuffer(wasDraw, StateCache::DrawFrameBuffer);
    }

    return true;
}

void FrameBuffer::Destroy() {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        glDeleteFramebuffers(1, &handle);
        SetGPUHandle(0);
    }
}

FrameBuffer::~FrameBuffer() {
    Destroy();
}

}
