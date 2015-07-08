#pragma once

#ifndef _MR_SERIALIZATION_H_
#define _MR_SERIALIZATION_H_

#include "Containers.hpp"
#include <string>

namespace mr {

template<typename T>
TStaticArray<unsigned char> Serialize(T* t) {
    unsigned char* c_ptr = new unsigned char [sizeof(T)];
    memcpy(&c_ptr[0], t, sizeof(T));
    return TStaticArray<unsigned char>(c_ptr, sizeof(T), true);
}

template<typename T>
T* DeSerialize(TStaticArray<unsigned char> T_bytes) {
    T* t = new T;
    memcpy(t, &(T_bytes.GetRaw()[0]), T_bytes.GetNum());
    return t;
}

template<typename T>
void DeSerializeTo(TStaticArray<unsigned char> T_bytes, T* dst) {
    memcpy(dst, &(T_bytes.GetRaw()[0]), T_bytes.GetNum());
}

/// Convert smth to binary format
template<typename t>
class SerializableBytes {
public:
    static TStaticArray<unsigned char> ToBytes(t*);
    static t* FromBytes(TStaticArray<unsigned char>);
};

template<typename t>
class ISerializableBytes {
public:
    TStaticArray<unsigned char> ToBytes();
    bool FromBytes(TStaticArray<unsigned char>);
};

/// Convert smth to text format
template<typename t>
class SerializableString {
public:
    static std::string ToString(t*);
    static t* FromString(const std::string&);
};

template<typename t>
class ISerializableString {
public:
    std::string ToString();
    bool FromString(const std::string&);
};

class SerializeMRTypes {
public:
    static TStaticArray<unsigned char> GPUBufferToBytes(class IBuffer*);
    static IBuffer* GPUBufferFromBytes(TStaticArray<unsigned char>);

    static TStaticArray<unsigned char> ShaderProgramToBytes(class IShaderProgram*);
    static IShaderProgram* ShaderProgramFromBytes(TStaticArray<unsigned char>);
};

}

#endif // _MR_SERIALIZATION_H_
