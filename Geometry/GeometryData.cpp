#include "GeometryData.hpp"

namespace MR {

VertexData::VertexData(const unsigned int& drawMode, const size_t& size, void* data, IVertexFormat* format)
    : _draw_mode(drawMode), _size(size), _memory(data), _format(format) {
}

VertexData::~VertexData() {
    if(_memory) {
        delete [] ((unsigned char*)_memory);
    }
}

IndexData::IndexData(const unsigned int& drawMode, const size_t& size, void* data, IIndexFormat* format)
    : _draw_mode(drawMode), _size(size), _memory(data), _format(format) {
}

IndexData::~IndexData() {
    if(_memory) {
        delete [] ((unsigned char*)_memory);
    }
}

}
