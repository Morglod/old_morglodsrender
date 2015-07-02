#pragma once

#include "ShaderTypes.hpp"

namespace mr {

class ShaderDescription {
public:
    enum ShaderTask : unsigned char {
        None = 0,
        Surface = 1,
        /*
        Shadow,
        ScreenSpace,
        */
        ShaderTasksNum
    };

    ShaderType shaderType = ShaderType::None;
    ShaderTask shaderTask = ShaderTask::None;
};

class ShaderGenerator {
public:
    ShaderGenerator();
    virtual ~ShaderGenerator();
};

}
