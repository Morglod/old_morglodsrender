#include "ShaderUBO.hpp"

#include "../Buffers/BuffersInterfaces.hpp"
#include "../Buffers/BufferManager.hpp"

#include "../Utils/Log.hpp"

namespace mr {

//TODO: Resize created buffer to fit newly created params

size_t ShaderUBO::GetGPUMem() {
    return (_buffer == nullptr) ? 0 : _buffer->GetGPUMem();
}

unsigned int ShaderUBO::GetGPUHandle() {
    return (_buffer == nullptr) ? 0 : _buffer->GetGPUHandle();
}

void ShaderUBO::Destroy() {
    auto& mgr = BufferManager::GetInstance();
    mgr.Delete(_buffer);
}

bool ShaderUBO::SetData(void* data, size_t const& size, bool dynamic) {
    if(_buffer == nullptr) {
        if(!CreateBuffer(size, dynamic)) return false;
    }
    return _buffer->Write(data, 0, 0, size, nullptr, nullptr);
}

bool ShaderUBO::CreateBuffer(size_t const& size, bool dynamic) {
    if(_buffer != nullptr && _buffer->GetGPUMem() >= size) return true;

    auto& mgr = BufferManager::GetInstance();

    //Reallocate if not enought memory
    if(_buffer->GetGPUMem() < size) {
        mgr.Delete(_buffer);
    }
    _buffer = mgr.CreateBuffer(size, (dynamic) ? BufferUsage::FastChange : BufferUsage::Static);

    //Failed create buffer
    if(_buffer == nullptr) {
        mr::Log::LogString("ShaderUBO::_CreateBuffer failed. Faield create buffer.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    return true;
}

bool ShaderUBO::SetParam(std::string const& name, void* value) {
    if(_buffer == nullptr) return false;
    ShaderUBOParams::Desc const& param = _params.params[name];
    return _buffer->Write(value, 0, param.offset, param.size, nullptr, nullptr);
}

bool ShaderUBO::AddParam(std::string const& name, size_t const& size, size_t const& offset, void* value) {
    _params.params[name] = ShaderUBOParams::Desc { size, offset };
    if(value != nullptr) return SetParam(name, value);
    return true;
}

bool ShaderUBO::AddParam(std::string const& name, ShaderUBOParams::Desc const& desc, void* value) {
    _params.params[name] = desc;
    if(value != nullptr) return SetParam(name, value);
    return true;
}

bool ShaderUBO::AddParams(std::vector<ShaderUBOParams::Named> const& params) {
    size_t baseOffset = _params.CalcTotalSize();
    for(size_t i = 0, n = params.size(); i < n; ++i) {
        ShaderUBOParams::Named const& named = params.at(i);
        if(!AddParam(named.name, named.size, baseOffset)) return false;
        baseOffset += named.size;
    }
    return true;
}

bool ShaderUBO::SetParams(std::vector<ShaderUBOParams::Named> const& params) {
    _params.params.clear();
    size_t baseOffset = 0;
    for(size_t i = 0, n = params.size(); i < n; ++i) {
        ShaderUBOParams::Named const& named = params.at(i);
        if(!AddParam(named.name, named.size, baseOffset)) return false;
        baseOffset += named.size;
    }
    return true;
}

ShaderUBO::~ShaderUBO() {}

}
