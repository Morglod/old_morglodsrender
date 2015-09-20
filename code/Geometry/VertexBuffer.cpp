#include "VertexBuffer.hpp"
#include "../Buffers/BufferManager.hpp"
#include "../Utils/Log.hpp"

namespace mr {

size_t VertexBuffer::CalcVertexCount() const {
    return _range.get()->GetSize() / _format->size;
}

bool VertexBuffer::Write(void* data, size_t const& dataSize, size_t const& offset) {
    auto* buffer = _range->GetBuffer();
    return buffer->Write(data, 0, offset + _range->GetOffset(), dataSize);
}

char* VertexBuffer::Map(int const& flags) {
    auto* buffer = _range->GetBuffer();
    if(buffer->IsMapped()) return nullptr;
    _mapping = buffer->Map(0, _range->GetSize(), flags);
    if(_mapping != nullptr) {
        return _mapping->Get();
    } else {
        mr::Log::LogString("Failed VertexBuffer::Map. Failed Buffer::Map", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
}

char* VertexBuffer::Map(size_t const& offset, size_t const& length, int const& flags) {
    auto* buffer = _range->GetBuffer();
    if(buffer->IsMapped()) return nullptr;
    _mapping = buffer->Map(_range->GetOffset()+offset, std::min(length, _range->GetSize()), flags);
    if(_mapping != nullptr) {
        return _mapping->Get();
    } else {
        mr::Log::LogString("Failed VertexBuffer::Map. Failed Buffer::Map", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
}

void VertexBuffer::UnMap() {
    if(_mapping == nullptr) return;
    _mapping->UnMap();
    _mapping = nullptr;
}

void VertexBuffer::Destroy() {
    if(_range != nullptr) {
        auto& mgr = BufferManager::GetInstance();
        IBuffer* buf = _range->GetBuffer();
        if(_ownBuffer) mgr.Destroy(buf);
    }
    _format = nullptr;
    _range = nullptr;
    _ownBuffer = false;
}

VertexBufferPtr VertexBuffer::Create(VertexFormatPtr const& format, size_t const& vertexesNum, BufferUsage const& usage) {
    const size_t size = format->size * vertexesNum;
    auto& mgr = BufferManager::GetInstance();
    auto* buffer = mgr.CreateBuffer(size, usage);
    if(buffer == nullptr) {
        mr::Log::LogString("Failed VertexBuffer::Create. Failed BufferManager::CreateBuffer", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
    auto vb = VertexBuffer::Create(format, buffer, vertexesNum, 0);
    if(vb != nullptr) vb->_ownBuffer = true;
    return vb;
}

VertexBufferPtr VertexBuffer::Create(VertexFormatPtr const& format, IBuffer* buffer, size_t const& vertexesNum, size_t const& offset) {
    const size_t size = format->size * vertexesNum;
    return VertexBuffer::Create(format, buffer->UseRange(size, offset));
}

VertexBufferPtr VertexBuffer::Create(VertexFormatPtr const& format, IBufferRangeHdlWPtr const& range) {
    VertexBuffer* vb = new VertexBuffer(format, range.lock(), false);
    return VertexBufferPtr(vb);
}

VertexBuffer::VertexBuffer(VertexFormatPtr const& format, IBufferRangeHdlPtr const& range, bool ownBuffer) : _range(range), _format(format), _ownBuffer(ownBuffer) {
}

VertexBuffer::~VertexBuffer() {
    Destroy();
}

}
