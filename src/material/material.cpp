#include "mr/material/MaterialShader.hpp"
#include "mr/vformat.hpp"
#include "src/shader/shader_code.hpp"
#include "mr/buffer.hpp"
#include "mr/string.hpp"
#include "mr/gl/types.hpp"
#include "mr/log.hpp"

#include "mr/pre/glew.hpp"

namespace {
mr::UniformBufferPtr _global_ubo = nullptr;
}

namespace mr {

BufferPtr MaterialShader::GetSystemUniformBuffer() {
    if(_global_ubo == nullptr) {
        Buffer::CreationFlags flags;
        flags.map_after_creation = true;
        flags.read = true;
        const size_t magic_glsl_padding = 20;
        _global_ubo = Buffer::Create(Memory::Own(new SysUniformData, sizeof(SysUniformData) + magic_glsl_padding), flags);
        if(_global_ubo == nullptr) {
            MR_LOG_ERROR(MaterialShader::GetSystemUniformBuffer, "failed create buffer");
            return nullptr;
        }
    }
    return _global_ubo;
}

bool MaterialShader::_Init(bool createBuffers) {
    if(_program == nullptr) {
        MR_LOG_ERROR(MaterialShader::_Init, "ShaderProgram not set");
        return false;
    }

    const uint32_t phandle = _program->GetId();
    if(phandle == 0) {
        MR_LOG_ERROR(MaterialShader::_Init, "ShaderProgram not created");
        return false;
    }

    int32_t num = 0;
    glGetProgramiv(phandle, GL_ACTIVE_UNIFORM_BLOCKS, &num);

    if(num == 0)
        MR_LOG_WARNING(MaterialShader::_Init, "0 active uniform blocks");
    else
        _ubos.reserve(num);

    for(int32_t i = 0; i < num; ++i) {
        char nameBuf[512];
        int32_t nameLen = 0;
        glGetActiveUniformBlockName(phandle, i, 512, &nameLen, nameBuf);
        std::string name(nameBuf, nameLen);

        BufferPtr buffer = nullptr;
        if(createBuffers) {
            int32_t bufSize;
            glGetActiveUniformBlockiv(phandle, i, GL_UNIFORM_BLOCK_DATA_SIZE, &bufSize);
            if(bufSize <= 0)
                MR_LOG_WARNING(MaterialShader::_Init, "bad uniform buffer size ["+std::to_string(i)+"]");
            else {
                if(name == SysUniformNameBlock) {
                    auto globalBuf = GetSystemUniformBuffer();
                    if(bufSize >= globalBuf->GetSize()) {
                        MR_LOG_WARNING(MaterialShader::_Init, "\"" + SysUniformNameBlock + "\" ("+std::to_string(bufSize)+") greather than mr::SysUniformData ("+std::to_string(globalBuf->GetSize())+")");
                    }
                    buffer = globalBuf;
                }
                else buffer = Buffer::Create(Memory::New(bufSize), Buffer::CreationFlags());

                if(buffer == nullptr)
                    MR_LOG_WARNING(MaterialShader::_Init, "failed create buffer for ["+std::to_string(i)+"]");
                else {
                    _program->SetUniformBuffer(SysUniformNameBlock, nullptr); // TODO nullptr fix
                }
            }
        }

        _ubos.push_back(/*sUBO{name, buffer}*/ nullptr); // TODO nullptr fix
    }

    return true;
}

void MaterialShader::SetUBO(std::string const& name, UniformBufferPtr const& buf) {
    for(int32_t i = 0, n = _ubos.size(); i < n; ++i) {
        if(_ubos[i].name == name) {
            _ubos[i].buffer = buf;
            return;
        }
    }
    MR_LOG_ERROR(MaterialShader::SetUBO, "uniform buffer not found \""+name+"\"");
}

MaterialShaderPtr MaterialShader::Create(ShaderProgramPtr const& program, bool createBuffers) {
    MaterialShaderPtr ms = MaterialShaderPtr(new MaterialShader);
    ms->_program = program;
    if(!ms->_Init(createBuffers)) {
        MR_LOG_ERROR(MaterialShader::Create, "init failed");
        return nullptr;
    }
    return ms;
}

}
