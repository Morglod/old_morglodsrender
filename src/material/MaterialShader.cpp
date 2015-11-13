#include "mr/material/MaterialShader.hpp"
#include "mr/vformat.hpp"
#include "src/shader/shader_code.hpp"
#include "mr/string.hpp"
#include "mr/gl/types.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

MaterialParamGroup::Changer MaterialParamGroup::Begin() {
    _params.clear();
    _size = 0;
}

std::string MaterialShaderGenerator::Vertex(VertexDeclPtr const& vdecl) {
    if(vdecl == nullptr) return "";
    std::string result = "";

    int acounter = 0;
    for(uint8_t ib = 0, nb = vdecl->GetBindpointsNum(); ib < nb; ++ib) {
        for(uint8_t ia = 0, na = vdecl->GetBindpointAttribsNum(ib); ia < na; ++ia) {
            const auto attrib = vdecl->GetAttribute(ib, ia);
            result += "layout(location = "+std::to_string(attrib.index)+") ";
            result += "in "; // TODO in/out
            result += VertexAttributeTypeString(attrib.datatype, attrib.components_num);
            result += " mr_attrib_" + std::to_string(acounter);
            result += ";\n";
            ++acounter;
        }
    }

    return result;
}

std::string MaterialShaderGenerator::Shader(MaterialShaderCfg const& cfg) {
    std::string result = common_shader_glsl_s;

    if(cfg.shaderType == ShaderType::None) ; // TODO error
    if(cfg.shaderType == ShaderType::Vertex) {
        ReplaceString(result, "{_MR_IS_VERTEX_SHADER_HERE}", "1");
        ReplaceString(result, "{_MR_IS_FRAGMENT_SHADER_HERE}", "0");
    } else if(cfg.shaderType == ShaderType::Fragment) {
        ReplaceString(result, "{_MR_IS_VERTEX_SHADER_HERE}", "0");
        ReplaceString(result, "{_MR_IS_FRAGMENT_SHADER_HERE}", "1");
    }

    ReplaceString(result, "{_MR_IS_CUSTOM_FUNC_HERE}", cfg.custom_func ? "1" : "0");

    ReplaceString(result, "{_MR_MATERIAL_HERE}", "");

    ReplaceString(result, "{_MR_VERTEX_IN_HERE}", Vertex(cfg.vertex_decl));

    ReplaceString(result, "{_MR_CUSTOM_FUNC_HERE}", cfg.custom_code);

    return result;
}

}
