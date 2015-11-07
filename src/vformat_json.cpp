#include "mr/vformat.hpp"
#include "mr/json.hpp"
#include "third/json/json.h"
#include "mr/log.hpp"
#include "src/mp.hpp"

/**

{
    "uid" : 123, // pointer to object in runtime, when exported
    "attribs" : [
        // Be careful with order of attributes
        {
            "components_num" : 2,
            "datatype" : 123,
            "bindpoint" : 0,
            "normalized" : 0
        }
    ]
}

**/

_MR_JSON_HEADER(mr::VertexDeclPtr)

template<>
MR_API bool mr::Json<mr::VertexDeclPtr>::_Export(mr::VertexDeclPtr& type, void* out) {
    MP_ScopeSample(mr::Json<mr::VertexDeclPtr>::_Export);
    ::Json::Value& j_root = *((::Json::Value*)out);

    j_root["uid"] = ::Json::Value((uint64_t)&type);

    ::Json::Value j_attribs;

    const uint8_t bindpointsNum = type->GetBindpointsNum();
    for(uint8_t ib = 0; ib < bindpointsNum; ++ib) {
        const uint8_t bindpointIndex = type->GetBindpointIndex(ib);
        const ::Json::Value j_bindpoint((uint32_t)bindpointIndex);
        const uint8_t bindpointAttribsNum = type->GetBindpointAttribsNum(ib);
        for(uint8_t ia = 0; ia < bindpointAttribsNum; ++ia) {
            const auto attrib = type->GetAttribute(ib, ia);

            ::Json::Value j_attrib;
            j_attrib["components_num"] = ::Json::Value((uint32_t)attrib.components_num);
            j_attrib["datatype"] = ::Json::Value((uint32_t)attrib.datatype);
            j_attrib["normalized"] = ::Json::Value((bool)attrib.normalized);
            j_attrib["bindpoint"] = j_bindpoint;

            j_attribs.append(j_attrib);
        }
    }

    j_root["attribs"] = j_attribs;

    return true;
}

template<>
MR_API bool mr::Json<mr::VertexDeclPtr>::_Import(mr::VertexDeclPtr& type, void* in) {
    MP_ScopeSample(mr::Json<mr::VertexDeclPtr>::_Import);
    ::Json::Value& j_root = *((::Json::Value*)in);

    if(!j_root.isObject()) {
        MR_LOG_ERROR(Json<VertexDeclPtr>::Import, "root value is not an Object");
        return false;
    }

    if(!j_root.isMember("attribs")) {
        MR_LOG_ERROR(Json<VertexDeclPtr>::Import, "\"attribs\" is not a member of root object");
        return false;
    }

    ::Json::Value j_attribs = j_root.get("attribs", ::Json::Value::null);
    if(!j_attribs.isArray()) {
        MR_LOG_ERROR(Json<VertexDeclPtr>::Import, "\"attribs\" is not an Array");
        return false;
    }

    auto changer = type->Begin();

    for(::Json::Value const& j_attrib : j_attribs) {
        if(!j_attrib.isObject()) {
            MR_LOG_ERROR(Json<VertexDeclPtr>::Import, "\"attribs\" element is not an Object");
            return false;
        }

        uint32_t components_num = 0;
        _MR_JSON_MEMBER(j_attrib, "components_num", isUInt, Json<VertexDeclPtr>::Import, components_num, asUInt);

        uint32_t datatype = 0;
        _MR_JSON_MEMBER(j_attrib, "datatype", isUInt, Json<VertexDeclPtr>::Import, datatype, asUInt);

        bool normalized = 0;
        _MR_JSON_MEMBER(j_attrib, "normalized", isBool, Json<VertexDeclPtr>::Import, normalized, asBool);

        uint32_t bindpoint = 0;
        _MR_JSON_MEMBER(j_attrib, "bindpoint", isUInt, Json<VertexDeclPtr>::Import, bindpoint, asUInt);

        changer.Custom((mr::DataType)datatype, (uint8_t)components_num, bindpoint, normalized);
    }

    changer.End();

    return true;
}
