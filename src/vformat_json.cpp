#include "mr/vformat.hpp"
#include "mr/json.hpp"
#include "third/json/json.h"
#include "mr/log.hpp"

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

#define MR_JSON_MEMBER(jsonValue, memberName, memberTypeCheckCall, callingMethod, outputVar, outputTypeCall) \
if(! jsonValue .isMember( memberName )) { \
    MR_LOG_ERROR( callingMethod , "\"" memberName "\" is not a member" ); \
    return false; \
} else { \
    ::Json::Value json_value = jsonValue .get( memberName , ::Json::Value::null); \
    if(! json_value . memberTypeCheckCall ()) { \
        MR_LOG_ERROR( callingMethod, "\"" memberName "\" member is not right type"); \
        return false; \
    } \
    outputVar = json_value . outputTypeCall (); \
}

namespace mr {
template<> bool mr::Json<mr::VertexDecl>::Export(mr::VertexDecl* type, std::ostream& out);
template<> bool mr::Json<mr::VertexDecl>::Import(mr::VertexDecl* type, std::istream& in);
}

template<>
MR_API bool mr::Json<mr::VertexDecl>::Export(mr::VertexDecl* type, std::ostream& out) {
    using namespace ::Json;

    /// j_* prefix - Json values or objects

    Value j_root;

    j_root["uid"] = Value((uint64_t)type);

    Value j_attribs;

    const uint8_t bindpointsNum = type->GetBindpointsNum();
    for(uint8_t ib = 0; ib < bindpointsNum; ++ib) {
        const uint8_t bindpointIndex = type->GetBindpointIndex(ib);
        const Value j_bindpoint((uint32_t)bindpointIndex);
        const uint8_t bindpointAttribsNum = type->GetBindpointAttribsNum(ib);
        for(uint8_t ia = 0; ia < bindpointAttribsNum; ++ia) {
            const auto attrib = type->GetAttribute(ib, ia);

            Value j_attrib;
            j_attrib["components_num"] = Value((uint32_t)attrib.components_num);
            j_attrib["datatype"] = Value((uint32_t)attrib.datatype);
            j_attrib["normalized"] = Value((bool)attrib.normalized);
            j_attrib["bindpoint"] = j_bindpoint;

            j_attribs.append(j_attrib);
        }
    }

    j_root["attribs"] = j_attribs;

    out << j_root;

    return true;
}

template<>
MR_API bool mr::Json<mr::VertexDecl>::Import(mr::VertexDecl* type, std::istream& in) {
    using namespace ::Json;

    /// j_* prefix - Json values or objects

    Value j_root;
    in >> j_root;

    if(!j_root.isObject()) {
        MR_LOG_ERROR(Json<VertexDecl>::Import, "root value is not an Object");
        return false;
    }

    if(!j_root.isMember("attribs")) {
        MR_LOG_ERROR(Json<VertexDecl>::Import, "\"attribs\" is not a member of root object");
        return false;
    }

    Value j_attribs = j_root.get("attribs", Value::null);
    if(!j_attribs.isArray()) {
        MR_LOG_ERROR(Json<VertexDecl>::Import, "\"attribs\" is not an Array");
        return false;
    }

    auto changer = type->Begin();

    for(Value const& j_attrib : j_attribs) {
        if(!j_attrib.isObject()) {
            MR_LOG_ERROR(Json<VertexDecl>::Import, "\"attribs\" element is not an Object");
            return false;
        }

        uint32_t components_num = 0;
        MR_JSON_MEMBER(j_attrib, "components_num", isUInt, Json<VertexDecl>::Import, components_num, asUInt);

        uint32_t datatype = 0;
        MR_JSON_MEMBER(j_attrib, "datatype", isUInt, Json<VertexDecl>::Import, datatype, asUInt);

        bool normalized = 0;
        MR_JSON_MEMBER(j_attrib, "normalized", isBool, Json<VertexDecl>::Import, normalized, asBool);

        uint32_t bindpoint = 0;
        MR_JSON_MEMBER(j_attrib, "bindpoint", isUInt, Json<VertexDecl>::Import, bindpoint, asUInt);

        changer.Custom((mr::DataType)datatype, (uint8_t)components_num, bindpoint, normalized);
    }

    changer.End();

    return true;
}
