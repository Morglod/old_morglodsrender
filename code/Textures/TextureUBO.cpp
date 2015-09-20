#include "TextureUBO.hpp"
#include "../Buffers/BufferManager.hpp"
#include "Texture.hpp"

namespace mr {

bool TextureUBO::Init(mu::ArrayHandle<Texture*> textures, const bool fastChange) {
    _textures = textures;
    const size_t bufferSize = sizeof(uint64_t) * textures.GetNum() * 2; // handles stored as <handle, none> (uint64_t[2])

    auto& bufMgr = mr::BufferManager::GetInstance();
    const auto bufferType = (fastChange) ? (BufferUsage::FastChange) : (BufferUsage::Static);
    _buffer = bufMgr.CreateBuffer(bufferSize, bufferType);

    bool updated = false;

    for(size_t i = 0, n = textures.GetNum(); i < n; ++i) {
        updated = UpdateTextureHandle(i);
    }

    if(!updated) {
        Destroy();
        return false;
    }

    return true;
}

bool TextureUBO::UpdateTextureHandle(size_t const& index) {
    uint64_t handle;
    if(!_textures.GetArray()[index]->GetResidentHandle(handle)) return false;
    // handles stored as <handle, none> (uint64_t[2])
    return _buffer->Write(&handle, 0, sizeof(uint64_t) * index * 2, sizeof(uint64_t), nullptr, nullptr);
}

bool TextureUBO::IndexOf(Texture* texture, size_t& outIndex) {
    if(_textures.GetNum() == 0) return false;
    Texture** textures = _textures.GetArray();
    for(size_t i = 0, n = _textures.GetNum(); i < n; ++i) {
        if(textures[i] == texture) {
            outIndex = i;
            return true;
        }
    }
    return false;
}

void TextureUBO::Destroy() {
    auto& bufMgr = mr::BufferManager::GetInstance();
    bufMgr.Destroy(_buffer);
}

size_t TextureUBO::GetGPUMem() {
    return (_buffer != nullptr) ? (_buffer->GetGPUMem()) : 0;
}

unsigned int TextureUBO::GetGPUHandle() {
    return (_buffer != nullptr) ? (_buffer->GetGPUHandle()) : 0;
}

IBuffer* TextureUBO::GetBuffer() const {
    return _buffer;
}

TextureUBO::TextureUBO() {}
TextureUBO::~TextureUBO() {}

}
