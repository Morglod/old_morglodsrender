#include "TextureStreamer2D.hpp"
#include "../Buffers/BufferManager.hpp"
#include "../Utils/Log.hpp"
#include "../StateCache.hpp"
#include "../Textures/Texture2D.hpp"

namespace mr {

bool TextureStreamer2D::Reset(Texture2D* newTarget, IDataSourcePtr const& dataSource) {
    Destroy();

    _target = newTarget;
    _dataSource = dataSource;
    _curPbo = 0;
    _synced = true;

    if(newTarget->GetGPUHandle() == 0) {
        mr::Log::LogString("Failed TextureStreamer2D::Reset. Target texture is not allocated.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    auto textureSizes = newTarget->GetSizesInfo(0);
    _pageSize = glm::uvec2(textureSizes.width, textureSizes.height / _totalIterations);

    GPUBufferManager& manager = GPUBufferManager::GetInstance();
    for(size_t i = 0; i < 2; ++i) {
        IGPUBuffer* buf = manager.CreateBuffer(IGPUBuffer::FastReadWrite, _pageSize.x * _pageSize.y * 3);

        if(buf == nullptr) {
            Destroy();
            mr::Log::LogString("Failed TextureStreamer2D::Reset. Failed create pbo", MR_LOG_LEVEL_ERROR);
            return false;
        }

        _pbos[i] = buf;
    }

    return true;
}

void TextureStreamer2D::Destroy() {
    GPUBufferManager& manager = GPUBufferManager::GetInstance();
    for(size_t i = 0; i < 2; ++i) {
        manager.Delete(_pbos[i]);
    }
}

bool TextureStreamer2D::Sync() {
    if(_synced) return true;

    StateCache* stateCache = StateCache::GetDefault();

    IGPUBuffer* was = nullptr;
    stateCache->ReBindBuffer(_pbos[_curPbo], StateCache::PixelUnpackBuffer, &was);
    _target->UpdateData(TextureData::FromMemory(mu::ArrayHandle<unsigned char>(0, 0, false, false), _pageSize, _target->GetDataFormat(), _target->GetDataType()), 0, glm::ivec2(_pageSize.x, _pageSize.y * _iterations));
    if(was) stateCache->BindBuffer(was, StateCache::PixelUnpackBuffer);

    _synced = true;
    return true;
}

bool TextureStreamer2D::Step() {
    if(!_synced) {
        mr::Log::LogString("Strange at TextureStreamer2D::Step. Not synced.", MR_LOG_LEVEL_WARNING);
        return true;
    }

    if(_dataSource->IsEnd()) return false;

    ++_curPbo;
    if(_curPbo == 2) _curPbo = 0;


    mr::Log::LogString("Step STREAMER - "+std::to_string(_curPbo), MR_LOG_LEVEL_ERROR);

    const size_t pageMemSize = _pageSize.x * _pageSize.y * 3;
    char data[pageMemSize];
    _dataSource->Read(data, pageMemSize);
    _pbos[_curPbo]->Write(data, 0, 0, pageMemSize, nullptr, nullptr);
    _synced = false;

    return true;
}

TextureStreamer2D::TextureStreamer2D(Texture2D* target, IDataSourcePtr const& dataSource) {
    for(size_t i = 0; i < 2; ++i) _pbos[i] = nullptr;
    Reset(target, dataSource);
}

TextureStreamer2D::~TextureStreamer2D() {
    Destroy();
}

}
