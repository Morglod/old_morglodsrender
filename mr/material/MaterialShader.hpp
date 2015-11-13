#pragma once

#include "../shader/shader.hpp"
#include <mr/gl/types.hpp>
/*
struct TestUBO {
    vec3 f1;
    float f2;
};
*/
#define MR_MATERIAL_PARAM(_group_, _x_) \
{   std::string str = std::string(#_x_) ; \
    size_t it = str.find_last_of("::"); \
    if(it != std::string::npos) str = str.substr(it+2); \
    _group_ .Param(str, GetTypeGL<decltype(_x_)>(nullptr)); \
}

// Example:
// MaterialParamGroup group;
// MR_MATERIAL_PARAM(group, TestUBO::f1);

namespace mr {

typedef std::shared_ptr<class VertexDecl> VertexDeclPtr;

class MaterialParamGroup final {
public:
    struct Changer final {
        friend class MaterialParamGroup;

        inline Changer& Name(std::string const& name);
        inline Changer& Param(std::string const& name, uint32_t datatype);
        void End();

        Changer(MaterialParamGroup& group);
    private:
        MaterialParamGroup& _group;
    };

    inline std::string GetName() const;
    Changer Begin();

private:
    struct sParam {
        uint32_t datatype;
        uint32_t size;
    };

    std::string _name;
    std::unordered_map<std::string, sParam> _params;
    size_t _size;
};

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

inline std::string MaterialParamGroup::GetName() const {
    return _name;
}

inline MaterialParamGroup::Changer& MaterialParamGroup::Changer::Name(std::string const& name) {
    _group._name = name;
}

inline MaterialParamGroup::Changer& MaterialParamGroup::Changer::Param(std::string const& name, uint32_t datatype) {
    _group._params.insert({name, sParam{datatype, sizeof_gl(datatype)}});
}

}
