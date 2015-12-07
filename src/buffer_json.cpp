#include "mr/buffer.hpp"
#include "mr/json.hpp"
#include "third/json/json.h"
#include "mr/log.hpp"
#include "mr/pre/glew.hpp"
#include "src/mp.hpp"

/**

{
    "uid" : 123, // pointer to object in runtime, when exported
    "size" : 123, // buffer's mem size in bytes
    "bytes" : [ ], // bytes
    "storage_flags" : 123,
    "resident_read" : True,
    "resident_write" : False,
    "map_offset" : 123,
    "map_length" : 123,
    "map_flags" : 123
}

**/

_MR_JSON_HEADER(mr::BufferPtr)

namespace mr {

template<>
MR_API bool mr::Json<mr::BufferPtr>::_Export(mr::BufferPtr& type, void* out) {
    MP_ScopeSample(mr::Json<mr::BufferPtr>::_Export);

    ::Json::Value& j_root = *((::Json::Value*)out);

    j_root["uid"] = ::Json::Value((uint64_t)type.get());

    BufferInfo buf;
    buf.Get(type->GetId());

    if(buf.size <= 0) {
        MR_LOG_WARNING(Json<BufferPtr>::Export, "exporting zero size buffer");
    }

    j_root["size"] = buf.size;
    j_root["storage_flags"] = buf.storage_flags;

    const auto residentState = type->GetResidentState();
    if(residentState.resident) {
        j_root["resident_read"] = ::Json::Value(residentState.read);
        j_root["resident_write"] = ::Json::Value(residentState.write);
    } else {
        j_root["resident_read"] = ::Json::Value(false);
        j_root["resident_write"] = ::Json::Value(false);
    }

    const auto mapState = type->GetMapState();
    j_root["map_offset"] = ::Json::Value(mapState.offset);
    j_root["map_length"] = ::Json::Value(mapState.length);
    j_root["map_flags"] = ::Json::Value(mapState.flags);

    MemoryPtr bufData = Memory::New(buf.size);
    if(!type->Read(bufData)) {
        MR_LOG_ERROR(Json<BufferPtr>::Export, "failed read buffer's data");
        return false;
    }

    uint8_t* bytesPtr = (uint8_t*)(bufData->GetPtr());

    ::Json::Value& j_bytes = (j_root["bytes"] = ::Json::Value());
    for(int32_t i = 0; i < buf.size; ++i) {
        j_bytes.append(::Json::Value(bytesPtr[i]));
    }

    return true;
}

template<>
MR_API bool mr::Json<mr::BufferPtr>::_Import(mr::BufferPtr& type, void* in) {
    MP_ScopeSample(mr::Json<mr::BufferPtr>::_Import);

    ::Json::Value& j_root = *((::Json::Value*)in);

    if(!j_root.isObject()) {
        MR_LOG_ERROR(Json<BufferPtr>::Import, "root value is not an Object");
        return false;
    }

    uint64_t uid = 0;
    _MR_JSON_MEMBER(j_root, "uid", isUInt64, Json<mr::BufferPtr>::Import, uid, asUInt64);
    ((void)uid); // use uid

    uint32_t buf_size = 0;
    _MR_JSON_MEMBER(j_root, "size", isUInt, Json<mr::BufferPtr>::Import, buf_size, asUInt);

    if(buf_size == 0) {
        MR_LOG_ERROR(Json<BufferPtr>::Import, "failed import zero size buffer");
        return false;
    }

    uint8_t* mem = new uint8_t[buf_size];
    ::Json::Value j_bytes;
    if(!(j_root.isMember("bytes") && (j_bytes = j_root.get("bytes", ::Json::Value::null)).isArray())) {
        MR_LOG_ERROR(Json<BufferPtr>::Import, "\"bytes\" is not a member, or not an array");
        return false;
    }

    for(uint32_t i = 0; i < buf_size; ++i) {
        if(!j_bytes.isValidIndex(i)) {
            MR_LOG_WARNING(Json<BufferPtr>::Import, "invalid index of \"bytes\" element, skip other");
            break;
        }
        const uint32_t byte = j_bytes.get(i, ::Json::Value::null).asUInt();
        if(byte > 255) {
            MR_LOG_ERROR(Json<BufferPtr>::Import, "\"bytes\" element is invalid (> 255)");
        }
        mem[i] = (uint8_t)byte;
    }

    uint32_t storage_flags = 0;
    _MR_JSON_MEMBER(j_root, "storage_flags", isUInt, Json<mr::BufferPtr>::Import, storage_flags, asUInt);

    bool resident_write = false, resident_read = false;
    _MR_JSON_MEMBER(j_root, "resident_read", isBool, Json<mr::BufferPtr>::Import, resident_read, asBool);
    _MR_JSON_MEMBER(j_root, "resident_write", isBool, Json<mr::BufferPtr>::Import, resident_write, asBool);

    uint32_t map_offset = 0, map_length = 0, map_flags = 0;
    _MR_JSON_MEMBER(j_root, "map_offset", isUInt, Json<mr::BufferPtr>::Import, resident_read, asUInt);
    _MR_JSON_MEMBER(j_root, "map_length", isUInt, Json<mr::BufferPtr>::Import, resident_write, asUInt);
    _MR_JSON_MEMBER(j_root, "map_flags", isUInt, Json<mr::BufferPtr>::Import, resident_write, asUInt);

    MemoryPtr buf_mem = Memory::Own(mem, buf_size);
    Buffer::CreationFlags creationFlags;
    creationFlags.client_storage = (bool)(storage_flags & GL_CLIENT_STORAGE_BIT);
    creationFlags.coherent = (bool)(storage_flags & GL_MAP_COHERENT_BIT);
    creationFlags.dynamic = (bool)(storage_flags & GL_DYNAMIC_STORAGE_BIT);
    creationFlags.persistent = (bool)(storage_flags & GL_MAP_PERSISTENT_BIT);
    creationFlags.read = (bool)(storage_flags & GL_MAP_READ_BIT);
    creationFlags.write = (bool)(storage_flags & GL_MAP_WRITE_BIT);

    type = Buffer::Create(buf_mem, creationFlags);

    if(type == nullptr) {
        MR_LOG_ERROR(Json<BufferPtr>::Import, "failed create buffer");
        return false;
    }

    if(!type->MakeResident(resident_read, resident_write)) {
        MR_LOG_WARNING(Json<BufferPtr>::Import, "failed make resident");
    }

    if(map_length != 0) {
        Buffer::MapOptFlags mapFlags;
        mapFlags.coherent = map_flags & GL_MAP_COHERENT_BIT;
        mapFlags.persistent = map_flags & GL_MAP_PERSISTENT_BIT;
        mapFlags.read = map_flags & GL_MAP_READ_BIT;
        mapFlags.write = map_flags & GL_MAP_WRITE_BIT;

        mapFlags.invalidate_range = map_flags & GL_MAP_INVALIDATE_RANGE_BIT;
        mapFlags.invalidate_buffer = map_flags & GL_MAP_INVALIDATE_BUFFER_BIT;
        mapFlags.flush_explicit = map_flags & GL_MAP_FLUSH_EXPLICIT_BIT;
        mapFlags.unsynchronized = map_flags & GL_MAP_UNSYNCHRONIZED_BIT;

        if(type->Map(map_length, mapFlags, map_offset).mem == nullptr) {
            MR_LOG_WARNING(Json<BufferPtr>::Import, "failed map buffer");
        }
    }

    return true;
}

}
