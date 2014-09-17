#pragma once

#ifndef _MR_RESOURCE_IO_INTERFACES_
#define _MR_RESOURCE_IO_INTERFACES_

#include "../Utils/Containers.hpp"

namespace MR {

class IResourceIOAsyncToBytes {
public:
    virtual bool Finished() = 0;
    virtual bool Good() = 0;
    virtual bool End() = 0;
    virtual void Cancel() = 0;
    virtual MR::TStaticArray<unsigned char> Result() = 0;

    virtual ~IResourceIOAsyncToBytes() {}
};

typedef std::shared_ptr<IResourceIOAsyncToBytes> IResourceIOAsyncToBytesPtr;

class IResourceIOAsyncFromBytes {
public:
    virtual bool Finished() = 0;
    virtual bool Good() = 0;
    virtual bool End() = 0;
    virtual void Cancel() = 0;

    virtual ~IResourceIOAsyncFromBytes() {}
};

typedef std::shared_ptr<IResourceIOAsyncFromBytes> IResourceIOAsyncFromBytesPtr;

class IResourceIO {
public:
    virtual MR::TStaticArray<unsigned char> ToBytes() = 0;
    virtual bool FromBytes(MR::TStaticArray<unsigned char>) = 0;

    virtual IResourceIOAsyncToBytesPtr ToBytesAsync() = 0;
    virtual IResourceIOAsyncFromBytesPtr FromBytesAsync() = 0;
};

}

#endif
