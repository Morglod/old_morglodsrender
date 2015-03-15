#include "GeometryManager.hpp"
#include "../Buffers/Buffers.hpp"
#include "GeometryBuffer.hpp"
#include "GeometryObject.hpp"
#include "GeometryDrawParams.hpp"

namespace mr {

template<>
mr::GeometryManager * mu::Singleton<mr::GeometryManager>::_singleton_instance = nullptr;

IGeometry* GeometryManager::PlaceGeometry(IVertexFormat* vertexFormat, void* vertexData, const size_t& vertexNum,
                             IIndexFormat* indexFormat, void* indexData, const size_t& indexNum,
                             const IGPUBuffer::Usage& usage, const IGeometryBuffer::DrawMode& drawMode) {
    AssertAndExec(vertexFormat != nullptr, return nullptr);
    AssertAndExec(vertexData != nullptr, return nullptr);
    AssertAndExec(vertexNum != 0, return nullptr);

    if(indexFormat){
        AssertAndExec(indexNum != 0, return nullptr);
    }

    const size_t vertexDataSize = vertexNum * vertexFormat->GetSize();
    const size_t indexDataSize = (indexFormat) ? (indexNum * indexFormat->GetSize()) : 0;

    if(_buffer_per_geom) {
        GPUBuffer* vertexBuffer = new mr::GPUBuffer();
        vertexBuffer->Allocate(usage, vertexDataSize);

        //Write thro mapping or by default
        auto vmapped = vertexBuffer->Map(0, vertexDataSize, IGPUBuffer::IMappedRange::Write | IGPUBuffer::IMappedRange::Invalidate | IGPUBuffer::IMappedRange::Unsynchronized);
        if(vmapped == nullptr) {
            vertexBuffer->Write(vertexData, 0, 0, vertexDataSize,  nullptr, nullptr);
        }
        else {
            memcpy(vmapped->Get(), vertexData, vertexDataSize);
            //vmapped->Flush();
            vmapped->UnMap();
        }

        GPUBuffer* indexBuffer = nullptr;
        if(indexFormat) {
            indexBuffer = new mr::GPUBuffer();
            indexBuffer->Allocate(usage, indexDataSize);

            //Write thro mapping or by default
            auto imapped = indexBuffer->Map(0, indexDataSize, IGPUBuffer::IMappedRange::Write | IGPUBuffer::IMappedRange::Invalidate | IGPUBuffer::IMappedRange::Unsynchronized);
            if(imapped == nullptr) {
                indexBuffer->Write(indexData, 0, 0, indexDataSize, nullptr, nullptr);
            }
            else {
                memcpy(imapped->Get(), indexData, indexDataSize);
                //imapped->Flush();
                imapped->UnMap();
            }
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

    //Write thro mapping or by default
    auto vmapped = virtualBuffer->Map(0, vertexDataSize, IGPUBuffer::IMappedRange::Write | IGPUBuffer::IMappedRange::Invalidate | IGPUBuffer::IMappedRange::Unsynchronized);
    if(vmapped == nullptr) {
        virtualBuffer->Write(vertexData, 0, 0, vertexDataSize, nullptr, &bufferedVertexDataInfo);
    } else {
        bufferedVertexDataInfo.buffer = virtualBuffer->GetRealBuffer();
        bufferedVertexDataInfo.size = vertexDataSize;
        bufferedVertexDataInfo.offset = virtualBuffer->GetRealOffset();
        memcpy(vmapped->Get(), vertexData, vertexDataSize);
        //vmapped->Flush();
        vmapped->UnMap();
    }

    if(indexDataSize) {
        //Write thro mapping or by default
        auto imapped = virtualBuffer->Map(vertexDataSize, indexDataSize, IGPUBuffer::IMappedRange::Write | IGPUBuffer::IMappedRange::Invalidate | IGPUBuffer::IMappedRange::Unsynchronized);
        if(imapped == nullptr) {
            virtualBuffer->Write(indexData, 0, vertexDataSize, indexDataSize, nullptr, &bufferedIndexDataInfo);
        }
        else {
            bufferedIndexDataInfo.buffer = virtualBuffer->GetRealBuffer();
            bufferedIndexDataInfo.size = indexDataSize;
            bufferedIndexDataInfo.offset = virtualBuffer->GetRealOffset() + vertexDataSize;
            memcpy(imapped->Get(), indexData, indexDataSize);
            //imapped->Flush();
            imapped->UnMap();
        }
    }

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
            if(_buffers[i].vFormat && !_buffers[i].vFormat->IsEqual(vertexFormat)) continue;
            if(_buffers[i].iFormat && !_buffers[i].iFormat->IsEqual(indexFormat)) continue;
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
