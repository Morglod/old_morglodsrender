#include "Serialization.hpp"
#include "Debug.hpp"
#include "../Buffers/Buffers.hpp"
#include "../Shaders/ShaderObjects.hpp"

namespace mr {

TStaticArray<unsigned char> SerializeMRTypes::GPUBufferToBytes(IGPUBuffer* buf) {
    Assert(!buf)
    Assert(buf->GetGPUMem() == 0)
    TStaticArray<unsigned char> _dst = TStaticArray<unsigned char>(new unsigned char [buf->GetGPUMem()], buf->GetGPUMem(), true);
    if(! buf->Read(&_dst.GetRaw()[0], 0, 0, buf->GetGPUMem())) {
        return TStaticArray<unsigned char>();
    }
    return _dst;
}

IGPUBuffer* SerializeMRTypes::GPUBufferFromBytes(TStaticArray<unsigned char> bytes) {
    Assert(bytes.GetNum() == 0)
    GPUBuffer* buf = new mr::GPUBuffer;
    buf->Allocate(IGPUBuffer::Static, bytes.GetNum());
    if(! buf->Write(bytes.GetRaw(), 0, 0, bytes.GetNum(), nullptr, nullptr)) {
        return nullptr;
    }
    return buf;
}

TStaticArray<unsigned char> SerializeMRTypes::ShaderProgramToBytes(IShaderProgram* program) {
    Assert(!program)
    Assert(program->GetGPUHandle() == 0)

    ShaderProgramCache cache = program->GetCache();
    if(cache.format == 0) return TStaticArray<unsigned char>();

    mr::TStaticArray<unsigned char> bytes = mr::TStaticArray<unsigned char>(new unsigned char[sizeof(unsigned int)+cache.data.GetNum()], sizeof(unsigned int)+cache.data.GetNum(), true);
    memcpy(bytes.GetRaw(), &cache.format, sizeof(unsigned int));
    memcpy(&bytes.GetRaw()[sizeof(unsigned int)], cache.data.GetArray(), cache.data.GetNum());
    return bytes;
}

IShaderProgram* SerializeMRTypes::ShaderProgramFromBytes(TStaticArray<unsigned char> bytes) {
    Assert(bytes.GetNum() == 0)

    unsigned int format = 0;
    unsigned char * data = new unsigned char [bytes.GetNum() - sizeof(unsigned int)];
    memcpy(&format, bytes.GetRaw(), sizeof(unsigned int));
    memcpy(data, &bytes.GetRaw()[sizeof(unsigned int)], bytes.GetNum());

    ShaderProgramCache cache = ShaderProgramCache(format, mu::ArrayHandle<unsigned char>(data, bytes.GetNum() - sizeof(unsigned int), true));
    return dynamic_cast<mr::IShaderProgram*>(mr::ShaderProgram::FromCache(cache));
}

}
