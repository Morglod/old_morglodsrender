#pragma once

#ifndef _MR_SHADER_BUILDER_H_
#define _MR_SHADER_BUILDER_H_

#include "ShaderInterfaces.hpp"

namespace MR {

class ShaderBuilder {
public:
    static std::string GenerateCode(const MR::IShaderProgram::Features& req, const MR::IShader::Type& type);
    static IShaderProgram* Need(const MR::IShaderProgram::Features& req);
    static IShader* FromFile(const std::string& file, const MR::IShader::Type& type);
};

}

#endif // _MR_SHADER_BUILDER_H_
