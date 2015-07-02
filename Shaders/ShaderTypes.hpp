#pragma once

namespace mr {

class ShaderTypeEnum {
public:
    enum Type {
        None = 0,
        Vertex = 0x8B31,
        Fragment = 0x8B30,
        Compute = 0x91B9,
        TessControl = 0x8E88,
        TessEvaluation = 0x8E87,
        Geometry = 0x8DD9,
        TypesNum = 8
    };
};

typedef ShaderTypeEnum::Type ShaderType;

}
