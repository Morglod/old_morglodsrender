#pragma once

#include "../shader/shader.hpp"

namespace mr {

typedef std::shared_ptr<class VertexDecl> VertexDeclPtr;

struct MaterialShaderCfg {
    ShaderType shaderType = ShaderType::Vertex;
    bool custom_func = false;
    std::string custom_code = "";
    VertexDeclPtr vertex_decl = nullptr;
};

class MR_API MaterialShaderGenerator {
public:
    static std::string Vertex(VertexDeclPtr const& vertexDecl);
    static std::string Shader(MaterialShaderCfg const& cfg);
};

class MR_API MaterialShader final {
public:
protected:
    //std::map<std::string, BufferPtr> ubos; // map ubos
};

}
