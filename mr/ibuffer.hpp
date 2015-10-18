#pragma once

#include "build.hpp"
#include "gl/types.hpp"
#include "memory.hpp"

#include <memory>

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;
typedef std::shared_ptr<class IndexBuffer> IndexBufferPtr;

class MR_API IndexBuffer final {
    friend class Draw;
public:
    inline uint32_t GetNum() const;
    inline IndexType GetDataType() const;
    static IndexBufferPtr Create(BufferPtr const& ibuf, IndexType const& datatype, uint32_t num);
    static IndexBufferPtr Create(MemoryPtr const& mem, IndexType const& datatype, uint32_t num);
protected:
    IndexBuffer(BufferPtr const& buf, MemoryPtr const& mem, IndexType const& dtype, uint32_t num);
    static bool _Bind(IndexBuffer* ib);
private:
    BufferPtr _buf;
    MemoryPtr _mem;
    IndexType _dtype;
    uint32_t _num;
};

inline uint32_t IndexBuffer::GetNum() const {
    return _num;
}

inline IndexType IndexBuffer::GetDataType() const {
    return _dtype;
}

}
