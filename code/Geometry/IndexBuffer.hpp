/**

IndexBuffer provides interface to work with indecies on gpu.

Example:

IndexFormatPtr format = ...; //describes format of Index
const int num = 10;
uint indecies[num]; //geometry data

IndexBufferPtr ib = IndexBuffer::Create(format, num);
uint* indexData = ib->MapIndex<uint>();
for(int i = 0; i < num; ++i) indexData[i] = indecies[i];
ib->UnMap();

**/

#pragma once

#include "../Buffers/BuffersInterfaces.hpp"
#include "GeometryFormats.hpp"

#include <memory>

namespace mr {

class IndexBuffer;
typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;

class IndexBuffer {
public:
    static IndexBufferPtr Create(IndexFormatPtr const& format, size_t const& indexNum, BufferUsage const& usage = BufferUsage::Static);
    static IndexBufferPtr Create(IndexFormatPtr const& format, IBuffer* buffer, size_t const& indexNum, size_t const& offset = 0);
    static IndexBufferPtr Create(IndexFormatPtr const& format, IBufferRangeHdlWPtr const& range);

    inline IndexFormatPtr GetFormat() const { return _format; }
    inline IBufferRangeHdlPtr GetBufferRange() const { return _range; }
    size_t CalcIndexCount() const;

    virtual bool Write(void* data, size_t const& dataSize, size_t const& offset = 0);

    virtual char* Map(int const& flags = IBuffer::IMappedRange::Write);
    virtual char* Map(size_t const& offset, size_t const& length, int const& flags = IBuffer::IMappedRange::Write);
    virtual void UnMap();
    inline IBuffer::IMappedRangePtr GetMapping() const { return _mapping; }

    template<typename TIndex>
    TIndex* MapIndex(int const& flags = IBuffer::IMappedRange::Write) {
        char* mem = Map(flags);
        if(mem == nullptr) return nullptr;
        return (TIndex*)(mem);
    }

    template<typename TIndex>
    TIndex* MapIndex(size_t const& indexNumOffset, size_t const& indexNum, int const& flags = IBuffer::IMappedRange::Write) {
        const auto isize = _format->dataType->size;
        char* mem = Map(indexNumOffset * isize, indexNum * isize, flags);
        if(mem == nullptr) return nullptr;
        return (TIndex*)(mem);
    }

    virtual void Destroy();

    virtual ~IndexBuffer();
protected:
    IndexBuffer(IndexFormatPtr const& format, IBufferRangeHdlPtr const& range, bool ownBuffer);

    IBufferRangeHdlPtr _range = nullptr;
    IndexFormatPtr _format = nullptr;
    IBuffer::IMappedRangePtr _mapping = nullptr;
    bool _ownBuffer = false;
};

}
