#pragma once

#ifndef _MR_CONFIG_CLASS_H_
#define _MR_CONFIG_CLASS_H_

#include <string>
#include <sstream>
#include <map>

namespace MR {
class Config {
public:
    virtual bool ParseFile(const std::string& path);
    bool Save() { return SaveTo(_file); }
    virtual bool SaveTo(const std::string& path);

    inline std::map<std::string, std::string> & GetMap() { return _map; }
    inline std::string& Get(const std::string& key) { return _map[key]; }
    inline void GetS(const std::string& key, std::stringstream * ss) { (*ss) << _map[key]; }
    inline void Set(const std::string& key, const std::string& value) { _map[key] = value; }
    inline bool Exists(const std::string& key) { return _map.count(key); }

    Config();
    ~Config();

    inline static Config FromFile(const std::string& file) {
        Config cfg;
        cfg.ParseFile(file);
        return cfg;
    }
protected:
    std::string _file;
    std::map<std::string, std::string> _map;
};
}

#endif // _MR_CONFIG_CLASS_H_
