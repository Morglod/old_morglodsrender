#pragma once

#include "build.hpp"
#include "gl/types.hpp"
#include "memory.hpp"

#include <memory>

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;
typedef std::shared_ptr<class IndexBuffer> IndexBufferPtr;

class MR_API IndexBuffer final {
    friend class Primitive;
    friend class _Alloc;
public:
    static IndexBufferPtr Create(BufferPtr const& ibuf, IndexDataType const& datatype, uint32_t num);
    static IndexBufferPtr Create(MemoryPtr const& mem, IndexDataType const& datatype, uint32_t num);
    bool Bind();

    inline BufferPtr GetBuffer() const;
    inline uint32_t GetNum() const;
    inline IndexDataType GetDataType() const;
protected:
    IndexBuffer(BufferPtr const& buf, MemoryPtr const& mem, IndexDataType const& dtype, uint32_t num);

private:
    BufferPtr _buf;
    MemoryPtr _mem;
    IndexDataType _dtype;
    uint32_t _num;
};

inline uint32_t IndexBuffer::GetNum() const {
    return _num;
}

inline IndexDataType IndexBuffer::GetDataType() const {
    return _dtype;
}

inline BufferPtr IndexBuffer::GetBuffer() const {
    return _buf;
}

}
