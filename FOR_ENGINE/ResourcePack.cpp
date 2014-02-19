#include "ResourcePack.hpp"

#include <zlib/zlib.h>
#include <iostream>
#include <fstream>

namespace MR {

PackResource::PackResource(const std::string& name, const bool& compressed, unsigned int size, unsigned char* data) :
    _name(name), _was_compressed(compressed), _data(data), _size(size)
{
    /*if(compressed){
        int err = deflateInit
    }*/
}

bool Pack::Load(){
    if(!_loaded){
        std::ifstream f(_file_path, std::ios::in | std::ios::binary);

        //First read num of files
        unsigned int _res_list_num = 0;
        f.read(reinterpret_cast<char*>(&_res_list_num), sizeof(unsigned int));

        //Read each file info
        for(unsigned int i = 0; i < _res_list_num; ++i){
            unsigned short nameSize = 0;
            f.read(reinterpret_cast<char*>(&nameSize), sizeof(unsigned short));

            char* cresName = new char[nameSize];
            f.read(cresName, nameSize);

            unsigned char icompressedFlag = 0;
            f.read(reinterpret_cast<char*>(&icompressedFlag), 1);

            unsigned int resSize = 0;
            f.read(reinterpret_cast<char*>(&resSize), sizeof(unsigned int));

            unsigned char * resData = new unsigned char[resSize];
            f.read(reinterpret_cast<char*>(&resData), resSize);

            _res_list.push_back(PackResource(std::string(cresName), icompressedFlag, resSize, resData));
        }

        f.close();
        _loaded = true;
    }
    return true;
}

bool Pack::UnLoad(){
    if(_loaded){

    }
    return false;
}

Pack::Pack(const std::string& file_path)
    : _file_path(file_path), _loaded(false) {

}

Pack::~Pack(){
    UnLoad();
}

}
