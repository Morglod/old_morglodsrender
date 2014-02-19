#pragma once

#ifndef _MR_RESOURCE_PACK_H_
#define _MR_RESOURCE_PACK_H_

#include <string>
#include <vector>

namespace MR {
class Pack;

class PackResource{
    friend class Pack;
public:
    inline std::string GetName(){return _name;}
    inline bool WasCompressed(){return _was_compressed;}
    inline unsigned char * GetData(){return _data;}
    inline unsigned int GetDataSize(){return _size;}
protected:
    std::string _name;
    bool _was_compressed;
    unsigned char* _data;
    unsigned int _size;
private:
    PackResource(){}
    PackResource(const std::string& name, const bool& compressed, unsigned int size, unsigned char* data);
};

class Pack{
    friend class PackResource;
public:
    inline std::string GetFilePath(){return _file_path;}
    inline bool IsLoaded(){return _loaded;}

    virtual bool Load();
    virtual bool UnLoad();

    Pack(const std::string& file_path);
    virtual ~Pack();
protected:
    std::string _file_path;
    bool _loaded;
    std::vector<PackResource> _res_list;
};

}

#endif // _MR_RESOURCE_PACK_H_
