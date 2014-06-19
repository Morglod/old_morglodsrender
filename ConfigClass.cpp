#include "ConfigClass.hpp"

#include <fstream>
#include <algorithm>

namespace MR {

bool Config::ParseFile(const std::string& path) {
    std::ifstream ifile(path);
    if(ifile.bad() || !ifile.is_open()) return false;
    _file = path;

    while(!ifile.eof()) {
        std::string buf = "";
        ifile >> buf;
        size_t split_pos = buf.find('=');
        if(split_pos == buf.npos) continue;
        std::string key = buf.substr(0, split_pos);
        std::string value = buf.substr(split_pos+1, buf.size()-split_pos);
        _map[key] = value;
    }

    ifile.close();
    return true;
}

bool Config::SaveTo(const std::string& path) {
    std::ofstream ofile(path);
    if(ofile.bad() || !ofile.is_open()) return false;

    for(auto it = _map.begin(); it != _map.end(); ++it){
        ofile << it->first << "=" << it->second << '\n';
    }

    return true;
}

Config::Config() : _file("") {
}

Config::~Config() {
}


}
