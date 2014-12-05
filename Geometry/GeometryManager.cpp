#include "GeometryManager.hpp"
#include "../Buffers/Buffers.hpp"
#include "GeometryBuffer.hpp"
#include "GeometryObject.hpp"
#include "GeometryDrawParams.hpp"

namespace mr {


IGeometry* GeometryManager::PlaceGeometry(IVertexFormat* vertexFormat, void* vertexData, const size_t& vertexNum,
                             IIndexFormat* indexFormat, void* indexData, const size_t& indexNum,
                             const IGPUBuffer::Usage& usage, const IGeometryBuffer::DrawMode& drawMode) {
    Assert(!vertexFormat)
    Assert(!vertexData)
    Assert(vertexNum == 0)

    if(indexFormat){
        Assert(!indexData)
        Assert(indexNum == 0)
    }

    const size_t vertexDataSize = vertexNum * vertexFormat->GetSize();
    const size_t indexDataSize = (indexFormat) ? (indexNum * indexFormat->GetSize()) : 0;

    if(_buffer_per_geom) {
        GPUBuffer* vertexBuffer = new mr::GPUBuffer();
        vertexBuffer->Allocate(usage, vertexDataSize);
        vertexBuffer->Write(vertexData, 0, 0, vertexDataSize,  nullptr, nullptr);

        GPUBuffer* indexBuffer = nullptr;
        if(indexFormat) {
            indexBuffer = new mr::GPUBuffer();
            indexBuffer->Allocate(usage, indexDataSize);
            indexBuffer->Write(indexData, 0, 0, indexDataSize, nullptr, nullptr);
        }

        mr::GeometryBuffer* geomBuffer = new mr::GeometryBuffer();
        if(!geomBuffer->Create  (
            IGeometryBuffer::CreationParams(
                dynamic_cast<IGPUBuffer*>(vertexBuffer), (indexFormat) ? (dynamic_cast<IGPUBuffer*>(indexBuffer)) : nullptr,
                vertexFormat, indexFormat,
                drawMode
            )
        )) {
            vertexBuffer->Destroy();
            if(indexBuffer) indexBuffer->Destroy();
            delete vertexBuffer;
            delete indexBuffer;
            delete geomBuffer;
            return nullptr;
        }

        return dynamic_cast<mr::IGeometry*>(
                    new mr::Geometry(
                        dynamic_cast<mr::IGeometryBuffer*>(geomBuffer), (indexFormat) ? GeometryDrawParams::DrawElements(0, indexDataSize / indexFormat->GetSize(), 0) : GeometryDrawParams::DrawArrays(0, vertexDataSize/vertexFormat->GetSize())
                    )
                );
    }

    FormatBuffer* fbuf = _RequestFormatBuffer(vertexFormat, vertexDataSize, indexFormat, indexDataSize, usage);
    VirtualGPUBuffer* virtualBuffer = fbuf->manager->Take(vertexDataSize+indexDataSize);
    mr::IGPUBuffer::BufferedDataInfo bufferedVertexDataInfo, bufferedIndexDataInfo;
    virtualBuffer->Write(vertexData, 0, 0, vertexDataSize, nullptr, &bufferedVertexDataInfo);

    if(indexDataSize) virtualBuffer->Write(indexData, 0, vertexDataSize, indexDataSize, nullptr, &bufferedIndexDataInfo);

    mr::GeometryBuffer* geomBuffer = new mr::GeometryBuffer();
    if(!geomBuffer->Create  (
        IGeometryBuffer::CreationParams(
            fbuf->manager->GetRealBuffer(), ((indexFormat) ? (fbuf->manager->GetRealBuffer()) : nullptr),
            vertexFormat, indexFormat,
            drawMode
        )
    )) {
        delete geomBuffer;
        return nullptr;
    }

    return dynamic_cast<mr::IGeometry*>(
                new mr::Geometry(
                    dynamic_cast<mr::IGeometryBuffer*>(geomBuffer),
                    (indexFormat) ? GeometryDrawParams::DrawElements(bufferedIndexDataInfo.offset / indexFormat->GetSize(), indexNum, bufferedVertexDataInfo.offset / vertexFormat->GetSize()) : GeometryDrawParams::DrawArrays(bufferedVertexDataInfo.offset / vertexFormat->GetSize(), vertexNum)
                )
            );
}

GeometryManager::FormatBuffer* GeometryManager::_RequestFormatBuffer(IVertexFormat* vertexFormat, const size_t& vertexDataSize,
                             IIndexFormat* indexFormat, const size_t& indexDataSize,
                             const IGPUBuffer::Usage& usage) {

    for(size_t i = 0; i < _buffers.size(); ++i) {
        if(_split_by_data_formats) {
            if(_buffers[i].vFormat && !_buffers[i].vFormat->Equal(vertexFormat)) continue;
            if(_buffers[i].iFormat && !_buffers[i].iFormat->Equal(indexFormat)) continue;
        }

        if((_buffers[i].usage == usage) &&
           (_buffers[i].manager))
        {
            if(_buffers[i].manager->GetFreeMemorySize() >= vertexDataSize+indexDataSize) {
                return &_buffers[i];
            }
        }
    }

    FormatBuffer formatBuf;
    formatBuf.buffer = new mr::GPUBuffer();
    formatBuf.buffer->Allocate(usage, std::max(vertexDataSize+indexDataSize, _max_buffer_size));
    formatBuf.iFormat = indexFormat;
    formatBuf.vFormat = vertexFormat;
    formatBuf.usage = usage;
    formatBuf.manager = new mr::VirtualGPUBufferManager(formatBuf.buffer, 0);
    _buffers.push_back(formatBuf);

    return &_buffers[_buffers.size()-1];
}

//5 mb per buffer
GeometryManager::GeometryManager() : _max_buffer_size(5242880), _buffer_per_geom(false), _split_by_data_formats(true) {
}

GeometryManager::~GeometryManager() {
    while(!_buffers.empty()) {
        FormatBuffer fbuf = _buffers.back();
        if(fbuf.manager) {
            delete fbuf.manager;
        }
        if(fbuf.buffer) {
            fbuf.buffer->Destroy();
            delete fbuf.buffer;
        }
        _buffers.pop_back();
    }
}

}
