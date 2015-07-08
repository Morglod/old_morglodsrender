#include "IndexBuffer.hpp"
#include "../Buffers/BufferManager.hpp"
#include "../Utils/Log.hpp"

namespace mr {

size_t IndexBuffer::CalcIndexCount() const {
    return _range.get()->GetSize() / _format->dataType->size;
}

bool IndexBuffer::Write(void* data, size_t const& dataSize, size_t const& offset) {
    auto* buffer = _range->GetBuffer();
    return buffer->Write(data, 0, offset + _range->GetOffset(), dataSize);
}

char* IndexBuffer::Map(int const& flags) {
    auto* buffer = _range->GetBuffer();
    if(buffer->IsMapped()) return nullptr;
    _mapping = buffer->Map(_range->GetOffset(), _range->GetSize(), flags);
    if(_mapping != nullptr) {
        return _mapping->Get();
    } else {
        mr::Log::LogString("Failed IndexBuffer::Map. Failed Buffer::Map", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
}

char* IndexBuffer::Map(size_t const& offset, size_t const& length, int const& flags) {
    auto* buffer = _range->GetBuffer();
    if(buffer->IsMapped()) return nullptr;
    _mapping = buffer->Map(_range->GetOffset()+offset, std::min(length, _range->GetSize()), flags);
    if(_mapping != nullptr) {
        return _mapping->Get();
    } else {
        mr::Log::LogString("Failed IndexBuffer::Map. Failed Buffer::Map", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
}

void IndexBuffer::UnMap() {
    if(_mapping == nullptr) return;
    _mapping->UnMap();
    _mapping = nullptr;
}

void IndexBuffer::Destroy() {
    if(_range != nullptr) {
        auto& mgr = BufferManager::GetInstance();
        IBuffer* buf = _range->GetBuffer();
        if(_ownBuffer) mgr.Delete(buf);
    }
    _format = nullptr;
    _range = nullptr;
    _ownBuffer = false;
}

IndexBufferPtr IndexBuffer::Create(IndexFormatPtr const& format, size_t const& indexNum, BufferUsage const& usage) {
    const size_t size = format->dataType->size * indexNum;
    auto& mgr = BufferManager::GetInstance();
    auto* buffer = mgr.CreateBuffer(size, usage);
    if(buffer == nullptr) {
        mr::Log::LogString("Failed IndexBuffer::Create. Failed BufferManager::CreateBuffer", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
    auto vb = IndexBuffer::Create(format, buffer, size, 0);
    if(vb != nullptr) vb->_ownBuffer = true;
    return vb;
}

IndexBufferPtr IndexBuffer::Create(IndexFormatPtr const& format, IBuffer* buffer, size_t const& indexNum, size_t const& offset) {
    const size_t size = format->dataType->size * indexNum;
    return IndexBuffer::Create(format, buffer->UseRange(size, offset));
}

IndexBufferPtr IndexBuffer::Create(IndexFormatPtr const& format, IBufferRangeHdlWPtr const& range) {
    IndexBuffer* vb = new IndexBuffer(format, range.lock(), false);
    return IndexBufferPtr(vb);
}

IndexBuffer::IndexBuffer(IndexFormatPtr const& format, IBufferRangeHdlPtr const& range, bool ownBuffer) : _range(range), _format(format), _ownBuffer(ownBuffer) {
}

IndexBuffer::~IndexBuffer() {
    Destroy();
}

}
