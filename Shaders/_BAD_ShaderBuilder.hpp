#pragma once

#ifndef _MR_SHADER_BUILDER_H_
#define _MR_SHADER_BUILDER_H_

#include "ShaderInterfaces.hpp"

namespace MR {

class ShaderBuilder {
public:
    class Params_Texture {
    public:
        enum Dimensions {
            D_1D,
            D_2D,
            D_3D
        };

        std::string name;
        Dimensions d;

        Params_Texture(const std::string& n, const Dimensions& d_) : name(n), d(d_) {}

        bool operator == (const ShaderBuilder::Params_Texture& p) const;
    };

    class Params_Uniform {
    public:
        std::string name;
        MR::IShaderUniform::Type type;

        bool operator == (const ShaderBuilder::Params_Uniform& p) const;
    };

    class Params {
    public:
        enum TextureMaps {
            NoMap = 0,
            ColorMap = 1,
            NormalMap,
            SpecularMap, //Reflections color and strength
            DisplacementMap, //Height, will not used if not enought texture units
            GlossMap,
            AOMap, //Baked ambient occlusion, will not used if not enought texture units
            OpacityMap,
            EnvironmentMap, //eg for IBL
            EmissionMap,
            TextureMapsNum
        };

        enum LightMode {
            UnLit = 0,
            Lit = 1,
            LightModsNum
        };

        enum VertexProcess {
            ToWorld = 0,
            ToScreen = 1,
            VertexProcessNum
        };

        std::vector<Params_Texture> textures;
        std::vector<Params_Uniform> uniforms;
        std::string customVertexCode, customVertexFuncName;
        std::string customFragmentCode, customFragmentFuncName;

        LightMode lightMode;
        VertexProcess vertexProcess;
        std::vector<TextureMaps> textureMaps;

        /*
            Custom vertex func runs at the end of vertex shader (Makes Final changes)
            void VertexFunc(in vec4 inPos, out vec4 outPos, in vec3 inNormal, out vec3 outNormal, in vec4 inColor, out vec4 outColor, in vec2 inTexCoord, out vec2 outTexCoord) {}

            Custom fragment func runs at the end of fragment shader (Makes Final changes)
            vec4 FragmentFunc(in vec4 inColor) {}
        */

        bool operator == (const ShaderBuilder::Params& p) const;
    };

    static std::string GenerateCode(const ShaderBuilder::Params& req, const MR::IShader::Type& type);
    static IShaderProgram* Need(const ShaderBuilder::Params& req);
    static IShader* FromFile(const std::string& file, const MR::IShader::Type& type);
    static IShaderProgram* SimpleMake(const std::string& vertex_shader, const std::string& fragment_shader);
};

}

#endif // _MR_SHADER_BUILDER_H_
