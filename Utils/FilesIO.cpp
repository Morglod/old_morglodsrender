#include "FilesIO.hpp"
#include "Singleton.hpp"

namespace MR {

bool FileWriter::IsOpen() {
    return _f.is_open();
}

std::string FileWriter::GetName() {
    return _name;
}

std::string FileWriter::GetPath() {
    return _path;
}

void FileWriter::Close() {
    _f.close();
}

void FileWriter::Write(const std::string& s) {
    _f << s;
}

FileWriter::~FileWriter() {
    Close();
}

FileWriter* FileWriter::Open(const std::string& fileName) {
    FileWriter* fw = new FileWriter();
    fw->_f.open(fileName);
    if(!FileUtils::Instance()->SplitPath(fileName, fw->_path, fw->_name)) {
        delete fw;
        return nullptr;
    }
    return fw;
}

bool FileReader::IsOpen() {
    return _f.is_open();
}

std::string FileReader::GetName() {
    return _name;
}

std::string FileReader::GetPath() {
    return _path;
}

void FileReader::Close() {
    _f.close();
}

std::string FileReader::ReadString() {
    std::string s = "";
    _f >> s;
    return s;
}

FileReader::~FileReader() {
    Close();
}

FileReader* FileReader::Open(const std::string& fileName, IFileUtils* fu) {
    std::string full_path = fu->FindFile(fileName);
    if(full_path == "") return nullptr;
    FileReader* fr = new FileReader();
    fr->_f.open(fileName);
    if(!fu->SplitPath(fileName, fr->_path, fr->_name)) {
        delete fr;
        return nullptr;
    }
    return fr;
}

FileReader* FileReader::Open(const std::string& fileName) {
    return FileReader::Open(fileName, FileUtils::Instance());
}

std::string FileUtils::FindFile(const std::string& fileName) {
    for(size_t i = 0; i < _searchPaths.size(); ++i){
        if(std::ifstream(_searchPaths[i] + "/" + fileName).is_open()) return _searchPaths[i] + "/" + fileName;
    }
    return "";
}

bool FileUtils::SplitPath(const std::string& fullPathToFile, std::string& path, std::string& fileName) {
    if(fullPathToFile == "") return false;
    auto it1 = fullPathToFile.find('\\');
    auto it2 = fullPathToFile.find('/');
    auto it = it1;
    if((it1 == fullPathToFile.npos) && (it2 == fullPathToFile.npos)) {
        path = "";
        fileName = fullPathToFile;
        return true;
    }
    if(it < it2) it = it2;
    path = fullPathToFile.substr(0, it);
    fileName = fullPathToFile.substr(it+1, fullPathToFile.size()-it);
    return true;
}

SingletonVar(FileUtils, new FileUtils());

FileUtils* FileUtils::Instance() {
    return SingletonVarName(FileUtils).Get();
}

}
