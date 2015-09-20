#include "GeometryManager.hpp"

#include "../Buffers/BufferManager.hpp"
#include "../Buffers/VirtualBuffer.hpp"
#include "GeometryBuffer.hpp"
#include "GeometryObject.hpp"
#include "GeometryDrawParams.hpp"
#include "GeometryFormats.hpp"

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

#include "../Utils/Debug.hpp"

namespace mr {

template<>
mr::GeometryManager * mu::Singleton<mr::GeometryManager>::_singleton_instance = nullptr;

IGeometry* GeometryManager::PlaceGeometry(VertexFormatPtr const& vertexFormat, void* vertexData, const size_t& vertexNum,
                             IndexFormatPtr const& indexFormat, void* indexData, const size_t& indexNum,
                             BufferUsage const& usage, DrawMode const& drawMode) {

    AssertAndExec(vertexFormat != nullptr, return nullptr);
    AssertAndExec(vertexData != nullptr, return nullptr);
    AssertAndExec(vertexNum != 0, return nullptr);

    if(indexFormat){
        AssertAndExec(indexNum != 0, return nullptr);
    }

    const size_t vertexDataSize = vertexNum * vertexFormat->size;
    const size_t indexDataSize = (indexFormat) ? (indexNum * indexFormat->dataType->size) : 0;

    if(_buffer_per_geom) {
        auto vertexBuffer = VertexBuffer::Create(vertexFormat, vertexNum, usage);
        if(vertexBuffer == nullptr) {
            mr::Log::LogString("Failed GeometryManager::PlaceGeometry. New vertexBuffer is null.", MR_LOG_LEVEL_ERROR);
            return nullptr;
        }
        vertexBuffer->Write(vertexData, vertexDataSize);

        IndexBufferPtr indexBuffer = nullptr;
        if(indexFormat) {
            indexBuffer = IndexBuffer::Create(indexFormat, indexNum, usage);
            if(indexBuffer == nullptr) {
                mr::Log::LogString("Failed GeometryManager::PlaceGeometry. New indexBuffer is null.", MR_LOG_LEVEL_ERROR);
                return nullptr;
            }
            indexBuffer->Write(indexData, indexDataSize);
        }

        IGeometryBufferPtr geomBuffer = IGeometryBufferPtr(static_cast<IGeometryBuffer*>(new mr::GeometryBuffer()));
        if(!geomBuffer->Create  (
            IGeometryBuffer::CreationParams(
                vertexBuffer, (indexFormat) ? (indexBuffer) : nullptr,
                vertexFormat, indexFormat,
                drawMode
            )
        )) {
            vertexBuffer->Destroy();
            indexBuffer->Destroy();
            return nullptr;
        }

        return static_cast<mr::IGeometry*>(
                    new mr::Geometry(
                        geomBuffer, (indexFormat) ? GeometryDrawParamsElements::Create(0, indexDataSize / indexFormat->dataType->size, 0) : GeometryDrawParamsArrays::Create(0, vertexDataSize/vertexFormat->size)
                    )
                );
    }
    return nullptr;
/*
    VertexFormatBuffer* vfbuf = _RequestVFBuffer(vertexFormat, vertexDataSize, usage);
    VirtualBuffer* vf_virtualBuffer = vfbuf->manager->Take(vertexDataSize);

    IndexFormatBuffer* ifbuf = _RequestIFBuffer(indexFormat, indexDataSize, usage);
    VirtualBuffer* if_virtualBuffer = ifbuf->manager->Take(indexDataSize);

    mr::IBuffer::BufferedDataInfo    bufferedVertexDataInfo,
                                        bufferedIndexDataInfo;

    //Write thro mapping or by default
    auto vmapped = vf_virtualBuffer->Map(0, vertexDataSize, IBuffer::IMappedRange::Write | IBuffer::IMappedRange::Invalidate | IBuffer::IMappedRange::Unsynchronized);
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
        auto imapped = if_virtualBuffer->Map(0, indexDataSize, IBuffer::IMappedRange::Write | IBuffer::IMappedRange::Invalidate | IBuffer::IMappedRange::Unsynchronized);
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

    //todo: fix, make VertexBuffer from virtual
    IGeometryBufferPtr geomBuffer = IGeometryBufferPtr(static_cast<IGeometryBuffer*>(new mr::GeometryBuffer()));
    if(!geomBuffer->Create  (
        IGeometryBuffer::CreationParams(
            vfbuf->manager->GetRealBuffer(), ((indexFormat) ? (ifbuf->manager->GetRealBuffer()) : nullptr),
            vertexFormat, indexFormat,
            drawMode
        )
    )) {
        return nullptr;
    }

    IGeometry* geom = static_cast<IGeometry*>(new mr::Geometry(
                    geomBuffer,
                    (indexFormat) ? GeometryDrawParamsElements::Create(bufferedIndexDataInfo.offset / indexFormat->dataType->size, indexNum, bufferedVertexDataInfo.offset / vertexFormat->size) : GeometryDrawParamsArrays::Create(bufferedVertexDataInfo.offset / vertexFormat->size, vertexNum)
                ));

    return geom;*/
}

GeometryManager::VertexFormatBuffer* GeometryManager::_RequestVFBuffer(VertexFormatPtr const& vertexFormat, const size_t& vertexDataSize, BufferUsage const& usage) {
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

    IBuffer* gpuBuffer = mr::BufferManager::GetInstance().CreateBuffer(std::max(vertexDataSize, _max_buffer_size), usage);
    if(gpuBuffer == nullptr) {
        mr::Log::LogString("Failed GeometryManager::_RequestFormatBuffer. New gpuBuffer is null.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    VertexFormatBuffer formatBuf;
    formatBuf.buffer = gpuBuffer;
    formatBuf.format = vertexFormat;
    formatBuf.usage = usage;
    formatBuf.manager = new mr::VirtualBufferManager(formatBuf.buffer, 0);
    _vertex_buffers.push_back(formatBuf);

    return &_vertex_buffers[_vertex_buffers.size()-1];
}

GeometryManager::IndexFormatBuffer* GeometryManager::_RequestIFBuffer(IndexFormatPtr const& indexFormat, const size_t& indexDataSize, BufferUsage const& usage) {
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

    IBuffer* gpuBuffer = mr::BufferManager::GetInstance().CreateBuffer(std::max(indexDataSize, _max_buffer_size), usage);
    if(gpuBuffer == nullptr) {
        mr::Log::LogString("Failed GeometryManager::_RequestFormatBuffer. New gpuBuffer is null.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    IndexFormatBuffer formatBuf;
    formatBuf.buffer = gpuBuffer;
    formatBuf.format = indexFormat;
    formatBuf.usage = usage;
    formatBuf.manager = new mr::VirtualBufferManager(formatBuf.buffer, 0);
    _index_buffers.push_back(formatBuf);

    return &_index_buffers[_index_buffers.size()-1];
}
//5 mb per buffer
GeometryManager::GeometryManager() : _max_buffer_size(5242880), _buffer_per_geom(true) {
}

GeometryManager::~GeometryManager() {
    mr::BufferManager& bufferMgr = BufferManager::GetInstance();

    while(!_vertex_buffers.empty()) {
        VertexFormatBuffer& fbuf = _vertex_buffers.back();
        if(fbuf.manager) {
            delete fbuf.manager;
        }
        bufferMgr.Destroy(fbuf.buffer);
        _vertex_buffers.pop_back();
    }
    while(!_index_buffers.empty()) {
        IndexFormatBuffer& fbuf = _index_buffers.back();
        if(fbuf.manager) {
            delete fbuf.manager;
        }
        bufferMgr.Destroy(fbuf.buffer);
        _index_buffers.pop_back();
    }
}

}
