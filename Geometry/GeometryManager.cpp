#include "GeometryManager.hpp"

#include "../Buffers/BufferManager.hpp"
#include "GeometryBuffer.hpp"
#include "GeometryObject.hpp"
#include "GeometryDrawParams.hpp"
#include "GeometryFormats.hpp"

namespace mr {

template<>
mr::GeometryManager * mu::Singleton<mr::GeometryManager>::_singleton_instance = nullptr;

IGeometry* GeometryManager::PlaceGeometry(VertexFormatPtr const& vertexFormat, void* vertexData, const size_t& vertexNum,
                             IndexFormatPtr const& indexFormat, void* indexData, const size_t& indexNum,
                             const IGPUBuffer::Usage& usage, const IGeometryBuffer::DrawMode& drawMode) {
    AssertAndExec(vertexFormat != nullptr, return nullptr);
    AssertAndExec(vertexData != nullptr, return nullptr);
    AssertAndExec(vertexNum != 0, return nullptr);

    if(indexFormat){
        AssertAndExec(indexNum != 0, return nullptr);
    }

    const size_t vertexDataSize = vertexNum * vertexFormat->size;
    const size_t indexDataSize = (indexFormat) ? (indexNum * indexFormat->dataType->size) : 0;

    if(_buffer_per_geom) {
        IGPUBuffer* vertexBuffer = mr::GPUBufferManager::GetInstance().CreateBuffer(usage, vertexDataSize);
        if(vertexBuffer == nullptr) {
            mr::Log::LogString("Failed GeometryManager::PlaceGeometry. New vertexBuffer is null.", MR_LOG_LEVEL_ERROR);
            return nullptr;
        }

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

        IGPUBuffer* indexBuffer = nullptr;
        if(indexFormat) {
            indexBuffer = mr::GPUBufferManager::GetInstance().CreateBuffer(usage, indexDataSize);
            if(indexBuffer == nullptr) {
                mr::Log::LogString("Failed GeometryManager::PlaceGeometry. New indexBuffer is null.", MR_LOG_LEVEL_ERROR);
                return nullptr;
            }

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
                        dynamic_cast<mr::IGeometryBuffer*>(geomBuffer), (indexFormat) ? GeometryDrawParams::DrawElements(0, indexDataSize / indexFormat->dataType->size, 0) : GeometryDrawParams::DrawArrays(0, vertexDataSize/vertexFormat->size)
                    )
                );
    }

    VertexFormatBuffer* vfbuf = _RequestVFBuffer(vertexFormat, vertexDataSize, usage);
    VirtualGPUBuffer* vf_virtualBuffer = vfbuf->manager->Take(vertexDataSize);

    IndexFormatBuffer* ifbuf = _RequestIFBuffer(indexFormat, indexDataSize, usage);
    VirtualGPUBuffer* if_virtualBuffer = ifbuf->manager->Take(indexDataSize);

    mr::IGPUBuffer::BufferedDataInfo    bufferedVertexDataInfo,
                                        bufferedIndexDataInfo;

    //Write thro mapping or by default
    auto vmapped = vf_virtualBuffer->Map(0, vertexDataSize, IGPUBuffer::IMappedRange::Write | IGPUBuffer::IMappedRange::Invalidate | IGPUBuffer::IMappedRange::Unsynchronized);
    if(vmapped == nullptr) {
        vf_virtualBuffer->Write(vertexData, 0, 0, vertexDataSize, nullptr, &bufferedVertexDataInfo);
    } else {
        bufferedVertexDataInfo.buffer = vf_virtualBuffer->GetRealBuffer();
        bufferedVertexDataInfo.size = vertexDataSize;
        bufferedVertexDataInfo.offset = vf_virtualBuffer->GetRealOffset();
        memcpy(vmapped->Get(), vertexData, vertexDataSize);
        //vmapped->Flush();
        vmapped->UnMap();
    }

    if(indexDataSize) {
        //Write thro mapping or by default
        auto imapped = if_virtualBuffer->Map(0, indexDataSize, IGPUBuffer::IMappedRange::Write | IGPUBuffer::IMappedRange::Invalidate | IGPUBuffer::IMappedRange::Unsynchronized);
        if(imapped == nullptr) {
            if_virtualBuffer->Write(indexData, 0, 0, indexDataSize, nullptr, &bufferedIndexDataInfo);
        }
        else {
            bufferedIndexDataInfo.buffer = if_virtualBuffer->GetRealBuffer();
            bufferedIndexDataInfo.size = indexDataSize;
            bufferedIndexDataInfo.offset = if_virtualBuffer->GetRealOffset();
            memcpy(imapped->Get(), indexData, indexDataSize);
            //imapped->Flush();
            imapped->UnMap();
        }
    }

    mr::GeometryBuffer* geomBuffer = new mr::GeometryBuffer();
    if(!geomBuffer->Create  (
        IGeometryBuffer::CreationParams(
            vfbuf->manager->GetRealBuffer(), ((indexFormat) ? (ifbuf->manager->GetRealBuffer()) : nullptr),
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
                    (indexFormat) ? GeometryDrawParams::DrawElements(bufferedIndexDataInfo.offset / indexFormat->dataType->size, indexNum, bufferedVertexDataInfo.offset / vertexFormat->size) : GeometryDrawParams::DrawArrays(bufferedVertexDataInfo.offset / vertexFormat->size, vertexNum)
                )
            );
}

GeometryManager::VertexFormatBuffer* GeometryManager::_RequestVFBuffer(VertexFormatPtr const& vertexFormat, const size_t& vertexDataSize, const IGPUBuffer::Usage& usage) {

    for(size_t i = 0; i < _vertex_buffers.size(); ++i) {
        if(_vertex_buffers[i].format && !_vertex_buffers[i].format->Equal(vertexFormat.get())) continue;
        if((_vertex_buffers[i].usage == usage) &&
           (_vertex_buffers[i].manager))
        {
            if(_vertex_buffers[i].manager->GetFreeMemorySize() >= vertexDataSize) {
                return &_vertex_buffers[i];
            }
        }
    }

    IGPUBuffer* gpuBuffer = mr::GPUBufferManager::GetInstance().CreateBuffer(usage, std::max(vertexDataSize, _max_buffer_size));
    if(gpuBuffer == nullptr) {
        mr::Log::LogString("Failed GeometryManager::_RequestFormatBuffer. New gpuBuffer is null.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    VertexFormatBuffer formatBuf;
    formatBuf.buffer = gpuBuffer;
    formatBuf.format = vertexFormat;
    formatBuf.usage = usage;
    formatBuf.manager = new mr::VirtualGPUBufferManager(formatBuf.buffer, 0);
    _vertex_buffers.push_back(formatBuf);

    return &_vertex_buffers[_vertex_buffers.size()-1];
}

GeometryManager::IndexFormatBuffer* GeometryManager::_RequestIFBuffer(IndexFormatPtr const& indexFormat, const size_t& indexDataSize, const IGPUBuffer::Usage& usage) {
    for(size_t i = 0; i < _index_buffers.size(); ++i) {
        if(_index_buffers[i].format && !_index_buffers[i].format->Equal(indexFormat.get())) continue;
        if((_index_buffers[i].usage == usage) &&
           (_index_buffers[i].manager))
        {
            if(_index_buffers[i].manager->GetFreeMemorySize() >= indexDataSize) {
                return &_index_buffers[i];
            }
        }
    }

    IGPUBuffer* gpuBuffer = mr::GPUBufferManager::GetInstance().CreateBuffer(usage, std::max(indexDataSize, _max_buffer_size));
    if(gpuBuffer == nullptr) {
        mr::Log::LogString("Failed GeometryManager::_RequestFormatBuffer. New gpuBuffer is null.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    IndexFormatBuffer formatBuf;
    formatBuf.buffer = gpuBuffer;
    formatBuf.format = indexFormat;
    formatBuf.usage = usage;
    formatBuf.manager = new mr::VirtualGPUBufferManager(formatBuf.buffer, 0);
    _index_buffers.push_back(formatBuf);

    return &_index_buffers[_index_buffers.size()-1];
}
//5 mb per buffer
GeometryManager::GeometryManager() : _max_buffer_size(5242880), _buffer_per_geom(false) {
}

GeometryManager::~GeometryManager() {
    while(!_vertex_buffers.empty()) {
        VertexFormatBuffer& fbuf = _vertex_buffers.back();
        if(fbuf.manager) {
            delete fbuf.manager;
        }
        if(fbuf.buffer) {
            fbuf.buffer->Destroy();
            delete fbuf.buffer;
        }
        _vertex_buffers.pop_back();
    }
    while(!_index_buffers.empty()) {
        IndexFormatBuffer& fbuf = _index_buffers.back();
        if(fbuf.manager) {
            delete fbuf.manager;
        }
        if(fbuf.buffer) {
            fbuf.buffer->Destroy();
            delete fbuf.buffer;
        }
        _index_buffers.pop_back();
    }
}

}
