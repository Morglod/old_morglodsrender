#include "ResourceSource.hpp"

namespace MR {

void FileResSource::Read(char* dest_buffer, size_t num) {
    if(IsReadable()) {
        fs.read(dest_buffer, num);
    }
}

void FileResSource::Write(char* src_buffer, size_t num) {
    if(IsWritable()) {
        fs.write(src_buffer, num);
    }
}

void FileResSource::Flush() {
    fs.flush();
}

size_t FileResSource::Size() {
    size_t curPos = fs.tellp();
    fs.seekp(fs.ate);
    size_t maxPos = fs.tellg();
    fs.seekp(curPos);
    return maxPos;
}

void FileResSource::Open(){
    std::ios_base::openmode om;
    if(IsReadable()) om = std::ios_base::in;
    if(IsWritable()) om |= std::ios_base::out;

    fs.open(GetSourcePath(), om);
    failed = !( fs.good() );
}

void FileResSource::Close(){
    fs.close();
}

FileResSource::FileResSource(bool CanRead, bool CanWrite, std::string SourcePath) : ResSource(CanRead, CanWrite, SourcePath) {
    Open();
}

FileResSource::~FileResSource() {
    fs.close();
}

ResSource* ResSource::File(std::string f) {
    return (new FileResSource(true, false, f));
}

}
