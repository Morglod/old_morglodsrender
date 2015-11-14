#include "mr/shader/ubo.hpp"
#include "mr/shader/program.hpp"
#include "mr/pre/glew.hpp"
#include "mr/log.hpp"
#include "mr/buffer.hpp"

#include <iostream>

namespace mr {

void UniformBufferDesc::sUniforms::Free() {
    if(arr != nullptr) {
        delete [] arr;
        arr = nullptr;
    }
    num = 0;
}

void UniformBufferDesc::sUniforms::Resize(int32_t num_) {
    Free();
    num = num_;
    arr = new Uniform[num_];
}

UniformBufferDesc::sUniforms::~sUniforms() {
    Free();
}

UniformBufferDescPtr UniformBufferDesc::Create(ShaderProgramPtr const& program, std::string const& ubo_name) {
    if(program == nullptr) {
        MR_LOG_ERROR(UniformBufferDesc::Create, "program is null");
        return nullptr;
    }
    uint32_t phandle = program->GetId();
    if(phandle == 0) {
        MR_LOG_ERROR(UniformBufferDesc::Create, "program not created");
        return nullptr;
    }

    {
        int32_t blocks_num = 0;
        glGetProgramiv(phandle, GL_ACTIVE_UNIFORM_BLOCKS, &blocks_num);
        if(blocks_num <= 0) {
            MR_LOG_WARNING(UniformBufferDesc::Create, "no active uniform blocks");
            return nullptr;
        }

        for(int32_t i = 0; i < blocks_num; ++i) {
            char nameBuf[512];
            int nameSize = 0;
            glGetActiveUniformBlockName(phandle, i, 512, &nameSize, nameBuf);

            std::string name = "";
            if(nameSize <= 0) {
                MR_LOG_WARNING(UniformBufferDesc::Create, "block data nonamed");
            } else if(ubo_name == std::string(nameBuf, nameSize)) {
                return Create(program, i);
            }
        }
    }

    return nullptr;
}

UniformBufferDescPtr UniformBufferDesc::Create(ShaderProgramPtr const& program, uint32_t ubo_index) {
    if(program == nullptr) {
        MR_LOG_ERROR(UniformBufferDesc::Create, "program is null");
        return nullptr;
    }
    uint32_t phandle = program->GetId();
    if(phandle == 0) {
        MR_LOG_ERROR(UniformBufferDesc::Create, "program not created");
        return nullptr;
    }

    UniformBufferDescPtr ubo = UniformBufferDescPtr(new UniformBufferDesc);
    ubo->_index = ubo_index;
    ubo->_program = program;

    glGetActiveUniformBlockiv(phandle, ubo_index, GL_UNIFORM_BLOCK_DATA_SIZE, &ubo->_size);
    if(ubo->_size == 0) {
        MR_LOG_ERROR(UniformBufferDesc::Create, "block data size is zero");
        return nullptr;
    }

    {
        char nameBuf[512];
        int nameSize = 0;
        glGetActiveUniformBlockName(phandle, ubo_index, 512, &nameSize, nameBuf);

        std::string name = "";
        if(nameSize <= 0) {
            MR_LOG_WARNING(UniformBufferDesc::Create, "block data nonamed");
        } else name = std::string(nameBuf, nameSize);

        ubo->_name = name;
    }

    {
        glGetActiveUniformBlockiv(phandle, ubo_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &ubo->_uniforms.num);
        if(ubo->_uniforms.num <= 0) {
            MR_LOG_WARNING(UniformBufferDesc::Create, "no active uniforms in uniform block");
        } else {
            ubo->_uniforms.Resize(ubo->_uniforms.num);

            uint32_t* indecies = new uint32_t[ubo->_uniforms.num];
            int32_t* offsets = new int32_t[ubo->_uniforms.num];

            glGetActiveUniformBlockiv(phandle, ubo_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (int*)indecies);
            glGetActiveUniformsiv(phandle, ubo->_uniforms.num, indecies, GL_UNIFORM_OFFSET, offsets);

            for(int32_t i = 0, n = ubo->_uniforms.num; i < n; ++i) {
                char nameBuf[512];
                int nameSize = 0;
                int unif_size = 0;
                unsigned int unif_type = 0;

                ubo->_uniforms.arr[i].index = indecies[i];
                ubo->_uniforms.arr[i].offset = offsets[i];

                glGetActiveUniform(phandle, ubo->_uniforms.arr[i].index, 512, &nameSize, &unif_size, &unif_type, nameBuf);

                if(nameSize <= 0) {
                    MR_LOG_WARNING(UniformBufferDesc::Create, "uniform nonamed");
                    ubo->_uniforms.arr[i].name = "";
                } else ubo->_uniforms.arr[i].name = std::string(nameBuf, nameSize);
            }

            delete [] indecies;
            delete [] offsets;
        }
    }

    return ubo;
}

bool UniformBufferDesc::Create(ShaderProgramPtr const& program, std::vector<UniformBufferDescPtr>& out_ubos) {
    if(program == nullptr) {
        MR_LOG_ERROR(UniformBufferDesc::Create, "program is null");
        return false;
    }
    uint32_t phandle = program->GetId();
    if(phandle == 0) {
        MR_LOG_ERROR(UniformBufferDesc::Create, "program not created");
        return false;
    }

    {
        int32_t blocks_num = 0;
        glGetProgramiv(phandle, GL_ACTIVE_UNIFORM_BLOCKS, &blocks_num);
        if(blocks_num <= 0) {
            MR_LOG_WARNING(UniformBufferDesc::Create, "no active uniform blocks");
            return true;
        }

        for(int32_t i = 0; i < blocks_num; ++i) {
            const auto ubo_desc = Create(program, i);
            if(ubo_desc != nullptr) {
                out_ubos.push_back(ubo_desc);
            }
        }
    }

    return true;
}

UniformBufferPtr UniformBuffer::Create(UniformBufferDescPtr const& desc) {
    UniformBufferPtr ubo = UniformBufferPtr(new UniformBuffer);
    ubo->_desc = desc;
    if(!ubo->_ResetBuffer()) {
        MR_LOG_ERROR(UniformBuffer::Create, "failed create buffer");
        return nullptr;
    }
    return ubo;
}

bool UniformBuffer::_ResetBuffer() {
    Buffer::CreationFlags flags; flags.map_after_creation = true;
    _buffer = Buffer::Create(Memory::Zero(_desc->GetSize()), flags);
    return (_buffer != nullptr);
}

void* UniformBuffer::At(int32_t arrayIndex) {
    return &((uint8_t*)_buffer->GetMapState().mem)[_desc->GetUniform(arrayIndex).offset];
}

void* UniformBuffer::At(std::string const& name) {
    UniformBufferDesc::Uniform u;
    int32_t arrayIndex;
    if(!_desc->FindUniformByName(name, u, arrayIndex)) return nullptr;
    return At(arrayIndex);
}

}

namespace std {

MR_API std::ostream& operator << (std::ostream& out, mr::UniformBufferDescPtr const& ubo_desc) {
    out << "UniformBufferDesc: \n";
    out << "\tname: " << ubo_desc->GetName() << " \n";
    out << "\tsize: " << ubo_desc->GetSize() << " \n";
    out << "\tindex: " << ubo_desc->GetIndex() << " \n";
    out << "\tuniforms: \n";
    for(int32_t i = 0, n = ubo_desc->GetUniformsNum(); i < n; ++i) {
        const auto uniform = ubo_desc->GetUniform(i);
        out << "\t\tname: " << uniform.name << " \n";
        out << "\t\tindex: " << uniform.index << " \n";
        out << "\t\toffset: " << uniform.offset << " \n";
    }
    return out;
}

}
