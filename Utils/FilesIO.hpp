#pragma once

#ifndef _MR_FILES_IO_H_

#include "Containers.hpp"
#include "Singleton.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

namespace mr {

class IFileUtils;

//File handle
class IFile {
public:
    virtual bool IsOpen() = 0;
    virtual std::string GetName() = 0; //with extension, for example "a.png"
    virtual std::string GetPath() = 0; //path to file, for example "Data/" or "" if in root dir
    virtual void Close() = 0;
    virtual ~IFile() { }
};

class FileWriter : public IFile {
public:
    bool IsOpen() override;
    std::string GetName() override;
    std::string GetPath() override;
    void Close() override;

    virtual void Write(const std::string& s);

    template<typename t>
    void WriteFrom(t& from){
        _f << from;
    }

    virtual ~FileWriter();

    static FileWriter* Open(const std::string& fileName);
protected:
    std::ofstream _f;
    std::string _name, _path;
private:
    FileWriter() {}
};

class FileReader : public IFile {
public:
    bool IsOpen() override;
    std::string GetName() override;
    std::string GetPath() override;
    void Close() override;

    virtual std::string ReadString();

    template<typename t>
    void ReadTo(t& to) {
        _f >> to;
    }

    virtual ~FileReader();

    static FileReader* Open(const std::string& fileName, IFileUtils* fu);
    static FileReader* Open(const std::string& fileName);
protected:
    std::ifstream _f;
    std::string _name, _path;
private:
    FileReader() {}
};

class IFileUtils {
public:
    virtual void AddSearchPath(const std::string& path) = 0;
    virtual void RemoveSearchPath(const std::string& path) = 0;
    virtual TStaticArray<std::string> GetSearchPaths() = 0; //sets (*paths_ptr) to ptr to list of paths

    virtual std::string FindFile(const std::string& fileName) = 0; //returns full path or null string
    virtual bool IsFileExist(const std::string& fileName) = 0;

    virtual bool SplitPath(const std::string& fullPathToFile, std::string& path, std::string& fileName) = 0;
};

class FileUtils : public IFileUtils, public mu::StaticSingleton<FileUtils> {
public:
    inline void AddSearchPath(const std::string& path) override { _searchPaths.push_back(path); }
    inline void RemoveSearchPath(const std::string& path) override{ auto it = std::find(_searchPaths.begin(), _searchPaths.end(), path); if(it==_searchPaths.end()) return; _searchPaths.erase(it); }
    inline TStaticArray<std::string> GetSearchPaths() override { return TStaticArray<std::string>(&_searchPaths[0], _searchPaths.size(), false); }

    std::string FindFile(const std::string& fileName) override;
    inline bool IsFileExist(const std::string& fileName) override { return (this->FindFile(fileName) != ""); }
    bool SplitPath(const std::string& fullPathToFile, std::string& path, std::string& fileName) override;
protected:
    std::vector<std::string> _searchPaths;
};

class FileIO {
public:
    static bool WriteBytes(std::string const& path, TStaticArray<unsigned char> bytes);
    static bool ReadBytes(std::string const& path, TStaticArray<unsigned char>& out_bytes);
    static size_t GetFileSize(std::string const& path);
};

}

#endif // _MR_FILES_IO_H_
