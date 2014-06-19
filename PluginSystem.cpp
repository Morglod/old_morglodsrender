#include "PluginSystem.hpp"
#include <windows.h>

namespace MR {

bool Plugin::Init(PluginSystem* sys) {
    return false;
}

void Plugin::Free() {
}

Plugin::Plugin() {}

Plugin::~Plugin() {
    Free();
    _sys->Remove(this);
}

bool PluginSystem::LoadFromModule(const std::string& path, Plugin** ptr_p) {
    if(ptr_p == 0) return false;
    HINSTANCE lib = LoadLibraryA(path.c_str());
    GetPluginInstanceProc proc = (GetPluginInstanceProc)GetProcAddress(lib, "GetPluginInstance");
    if(proc == 0) {
        FreeLibrary(lib);
        return false;
    }
    *ptr_p = proc(this);
    if((*ptr_p) == 0) {
        FreeLibrary(lib);
        return false;
    }
    return true;
}

}
