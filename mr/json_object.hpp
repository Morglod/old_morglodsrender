#pragma once

#include "build.hpp"

#include "third/json/json.h"

#include <string>
#include <unordered_map>
#include <iostream>

namespace mr {

template<typename T>
class MR_API JsonObject {
private:
    class MR_API BaseFieldRef {
    public:
        size_t offset;

        inline bool Export(::Json::Value& root, std::string const& name, FieldT* valuePtr);
        virtual bool Import(::Json::Value& root, std::string const& name, FieldT* valuePtr) = 0;
    };
    typedef std::shared_ptr<BaseFieldRef> BaseFieldRefPtr;

    template<typename FieldT>
    class MR_API FieldRef {
    public:
        bool Import(::Json::Value& root, std::string const& name, FieldT* valuePtr) override;
    };

public:
    inline void Field(std::string const& name, size_t memOffset);
    inline bool Export(::Json::Value& out);
    inline bool Import(::Json::Value& in);

    inline JsonObject(T& object);
private:

    T& objectRef;
    std::unordered_map<std::string, BaseFieldRefPtr) fields;
};

}

template<typename T>
inline JsonObject<T>::JsonObject(T& object) : objectRef(object) {}

template<typename T>
inline void mr::JsonObject<T>::Field(std::string const& name, size_t memOffset) {
    FieldRef* fr = MR_NEW(FieldRef);
    fr->offset = memOffset;
    fields[name] = BaseFieldRefPtr((BaseFieldRef*)fr);
}

template<typename T>
inline bool mr::JsonObject<T>::BaseFieldRef::Export(::Json::Value& root, std::string const& name, FieldT* valuePtr) {
    root[name] = ::Json::Value(*valuePtr);
}

template<typename T>
inline bool mr::JsonObject<T>::Export(::Json::Value& out) {
}

template<typename T>
inline bool mr::JsonObject<T>::Import(::Json::Value& in) {
}

#define _MR_DEF_JSON_IMPORT(ctype, checker, converter) \
template<typename T> \
bool mr::JsonObject<T>::FieldRef<ctype>::Import(::Json::Value& root, std::string const& name, FieldT* valuePtr) { \
    _MR_JSON_MEMBER(root, name, checker, JsonObject::Import, *valuePtr, converter); \
    return true; \
}

_MR_DEF_JSON_IMPORT(int32_t, isInt, asInt)
_MR_DEF_JSON_IMPORT(uint32_t, isUInt, asUInt)
_MR_DEF_JSON_IMPORT(int64_t, isInt64, asInt64)
_MR_DEF_JSON_IMPORT(uint64_t, isUInt64, asUInt64)
_MR_DEF_JSON_IMPORT(float, isFloat, asFloat)
_MR_DEF_JSON_IMPORT(double, isDouble, asDouble)
_MR_DEF_JSON_IMPORT(bool, isBool, asBool)
_MR_DEF_JSON_IMPORT(std::string, isString, asString)

#undef _MR_DEF_JSON_IMPORT
