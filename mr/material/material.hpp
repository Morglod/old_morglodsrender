#pragma once

#include "mr/shader/program.hpp"
#include "mr/pre/glew.hpp"
#include "mr/pre/glm.hpp"
#include "mr/string.hpp"
#include "mr/shader/ubo.hpp"

#include <unordered_map>

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;
typedef std::shared_ptr<class Material> MaterialPtr;
typedef std::shared_ptr<class Texture2D> Texture2DPtr;

constexpr const char* MaterialDiffuseTex = "mr_material.diffuseTex";
constexpr const char* MaterialNormalTex = "mr_material.normalTex";
constexpr const char* MaterialSpecularTex = "mr_material.specularTex";

class MR_API Material final {
    friend class _Alloc;
public:
    template<typename T>
    inline void SetUniform(std::string const& name, T const& value) const {
        *((T*)(_uniforms.at(name).ptr)) = value;
    }

    template<typename T>
    inline T GetUniform(std::string const& name) const {
        return *((T*)(_uniforms.at(name).ptr));
    }

    void UpdateShaderUniforms();

    inline ShaderProgramPtr GetProgram() const;

    /**
        textures = <uniform name, texture to set>
    **/
    static MaterialPtr Create(ShaderProgramPtr const& program, std::vector<std::string> const& uniformNames = std::vector<std::string>(), std::unordered_map<std::string, Texture2DPtr> const& textures = std::unordered_map<std::string, Texture2DPtr>());

    ~Material();
protected:
    Material() = default;

    struct sTexture {
        uint64_t residentHandle = 0;
        Texture2DPtr texture = nullptr;
        std::string uniformName;
    };

    uint32_t _textures_num = 0;
    sTexture* _textures = nullptr;

    ShaderProgramPtr _program;

    std::unordered_map<std::string, UniformRefAny> _uniforms; // <uniform name, ref>

};

inline ShaderProgramPtr Material::GetProgram() const {
    return _program;
}

}
