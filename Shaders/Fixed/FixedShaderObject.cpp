#include "FixedShaderObject.hpp"

std::string __MR_FixedShaderProgram_VertexShader =
"";

std::string __MR_FixedShaderProgram_FragmentShader =
"";

namespace MR {

void FixedShaderProgram::SetTexture(TextureMaps const& texMap, ITexture* tex) {
    _maps[(size_t)texMap] = tex;
}

void FixedShaderProgram::SetDisplacementTech(DisplacementTech const& tech) {
}

bool FixedShaderProgram::Use() {
}

FixedShaderProgram::FixedShaderProgram() {
    for(size_t i = 0; i < TextureMapsNum; ++i) {
        _maps[i] = nullptr;
    }
}

FixedShaderProgram::~FixedShaderProgram() {
}

}
