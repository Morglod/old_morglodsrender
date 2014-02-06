#pragma once

#ifndef _MR_RESOURCE_SOURCE_H_
#define _MR_RESOURCE_SOURCE_H_

#include "pre.hpp"

namespace MR {
class ResSource {
private:
    bool readable;
    bool writable;
    std::string source_path;
    ResSource() {}
protected:
    bool failed;
public:
    virtual void Read(char* dest_buffer, size_t num) = 0;
    virtual void Write(char* src_buffer, size_t num) = 0;
    virtual void Flush() {}
    inline bool IsReadable() { return readable; }
    inline bool IsWritable() { return writable; }
    virtual void Open() = 0;
    virtual void Close() = 0;
    inline bool Failed() { return failed; }
    inline std::string GetSourcePath() { return source_path; }
    virtual size_t Size() = 0;

    ResSource(bool CanRead, bool CanWrite, std::string SourcePath) : readable(CanRead), writable(CanWrite), source_path(SourcePath), failed(false) { }
    virtual ~ResSource() {}

    static ResSource* File(std::string f);
};

class FileResSource : public virtual ResSource {
private:
    std::fstream fs;
public:
    void Read(char* dest_buffer, size_t num);
    void Write(char* src_buffer, size_t num);
    void Flush();
    void Open();
    void Close();
    size_t Size();

    FileResSource(bool CanRead, bool CanWrite, std::string SourcePath);
    virtual ~FileResSource();
};
}

#endif
