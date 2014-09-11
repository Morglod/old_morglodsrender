#pragma once

#ifndef _MR_SERIALIZATION_H_
#define _MR_SERIALIZATION_H_

#include "Containers.hpp"
#include <string>

namespace MR {

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

class IGPUBuffer;
class IShaderProgram;
class Serialize {
public:
    static TStaticArray<unsigned char> GPUBufferToBytes(IGPUBuffer*);
    static IGPUBuffer* GPUBufferFromBytes(TStaticArray<unsigned char>);

    static TStaticArray<unsigned char> ShaderProgramToBytes(IShaderProgram*);
    static IShaderProgram* ShaderProgramFromBytes(TStaticArray<unsigned char>);
};

}

#endif // _MR_SERIALIZATION_H_
