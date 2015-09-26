#pragma once

#include "build.hpp"

#include <memory>

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;
typedef std::shared_ptr<class IndexBuffer> IndexBufferPtr;

enum class IndexType : uint32_t {
    UShort = 0x1403,
    UInt = 0x1405
};

class MR_API IndexBuffer final {
    friend class Draw;
public:
    inline uint32_t GetNum() const { return _num; }
    inline IndexType GetDataType() const { return _dtype; }
    static IndexBufferPtr Create(BufferPtr const& ibuf, IndexType const& datatype, uint32_t num);
    static IndexBufferPtr Create(void* array, IndexType const& datatype, uint32_t num);
protected:
    IndexBuffer(BufferPtr const& buf, void* mem, IndexType const& dtype, uint32_t num);
    static bool _Bind(IndexBuffer* ib);
private:
    BufferPtr _buf;
    void* _mem;
    IndexType _dtype;
    uint32_t _num;
};

}
