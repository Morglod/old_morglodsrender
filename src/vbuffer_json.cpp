#include "mr/vbuffer.hpp"
#include "mr/json.hpp"
#include "third/json/json.h"
#include "mr/log.hpp"
#include "mr/pre/glew.hpp"
#include "src/mp.hpp"

/**

{
    "uid" : 123, // pointer to object in runtime, when exported
    "num" : 123, // verticies num
    "buffer" : { Buffer object },
    "vdecl" : { VertexDecl object }
}

**/

_MR_JSON_HEADER(mr::VertexBufferPtr)

namespace mr {

template<>
MR_API bool mr::Json<mr::VertexBufferPtr>::_Export(mr::VertexBufferPtr& type, void* out) {
    MP_ScopeSample(mr::Json<mr::VertexBufferPtr>::_Export);

    ::Json::Value& j_root = *((::Json::Value*)out);

    j_root["uid"] = ::Json::Value((uint64_t)type.get());
    j_root["num"] = ::Json::Value((uint32_t)type->GetNum());

    auto buffer = type->GetBuffer();
    auto vdecl = type->GetVertexDecl();

    ::Json::Value j_buffer, j_vdecl;
    if(!mr::Json<mr::BufferPtr>::_Export(buffer, &j_buffer)) {
        MR_LOG_ERROR(mr::Json<mr::VertexBufferPtr>::_Export, "failed export VertexBuffer's buffer");
        return false;
    }
    if(!mr::Json<mr::VertexDeclPtr>::_Export(vdecl, &j_vdecl)) {
        MR_LOG_ERROR(mr::Json<mr::VertexBufferPtr>::_Export, "failed export VertexBuffer's vertex declaration");
        return false;
    }

    j_root["buffer"] = j_buffer;
    j_root["vdecl"] = j_vdecl;

    return true;
}

template<>
MR_API bool mr::Json<mr::VertexBufferPtr>::_Import(mr::VertexBufferPtr& type, void* in) {
    MP_ScopeSample(mr::Json<mr::VertexBufferPtr>::_Import);

    ::Json::Value& j_root = *((::Json::Value*)in);

    if(!j_root.isObject()) {
        MR_LOG_ERROR(Json<VertexBufferPtr>::_Import, "root value is not an Object");
        return false;
    }

    uint64_t uid = 0;
    _MR_JSON_MEMBER(j_root, "uid", isUInt64, Json<mr::VertexBufferPtr>::_Import, uid, asUInt64);

    uint32_t num = 0;
    _MR_JSON_MEMBER(j_root, "num", isUInt, Json<mr::VertexBufferPtr>::_Import, num, asUInt);

    if(num == 0) {
        MR_LOG_WARNING(Json<VertexBufferPtr>::_Import, "zero verticies");
    }

    if(!j_root.isMember("buffer")) {
        MR_LOG_ERROR(Json<VertexBufferPtr>::_Import, "\"buffer\" is not a Member");
        return false;
    }

    if(!j_root.isMember("vdecl")) {
        MR_LOG_ERROR(Json<VertexBufferPtr>::_Import, "\"vdecl\" is not a Member");
        return false;
    }

    ::Json::Value j_buffer = j_root.get("buffer", ::Json::Value::null),
                  j_vdecl = j_root.get("vdecl", ::Json::Value::null);

    mr::BufferPtr buffer;
    if(!mr::Json<mr::BufferPtr>::_Import(buffer, &j_buffer)) {
        MR_LOG_ERROR(Json<VertexBufferPtr>::_Import, "failed import from \"vdecl\"");
        return false;
    }

    mr::VertexDeclPtr vdecl = mr::VertexDecl::Create();
    if(!mr::Json<mr::VertexDeclPtr>::_Import(vdecl, &j_vdecl)) {
        MR_LOG_ERROR(Json<VertexBufferPtr>::_Import, "failed import from \"vdecl\"");
        return false;
    }

    type = mr::VertexBuffer::Create(buffer, vdecl, num);

    return true;
}

}

