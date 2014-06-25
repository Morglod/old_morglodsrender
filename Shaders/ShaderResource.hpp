#pragma once

#ifndef _MR_SHADER_H_
#define _MR_SHADER_H_

#include "ShaderObjects.hpp"

#include <map>

namespace MR {
/*
class ShaderProgramResource : public ShaderProgram, public virtual Resource {
protected:
    bool _Loading() override;
    void _UnLoading() override;
};

class ShaderManager : public virtual MR::ResourceManager {
public:
    virtual Resource* Create(const std::string& name, const std::string& source);
    inline ShaderProgramResource* NeedShader(const std::string& source);

    ShaderManager() : ResourceManager() {}
    virtual ~ShaderManager() {}

    static ShaderManager* Instance() { return nullptr; }
    static void DestroyInstance() {}
};
}
*/
/** INLINES **/
/*
MR::ShaderProgramResource* MR::ShaderManager::NeedShader(const std::string& source) {
    return dynamic_cast<ShaderProgramResource*>(Need(source));
}*/
}
#endif
