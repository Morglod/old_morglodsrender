#pragma once

#include "mr/build.hpp"
#include <memory>
#include <vector>

namespace mr {

typedef std::shared_ptr<class UniformBuffer> UniformBufferPtr;
typedef std::shared_ptr<class UniformBufferDesc> UniformBufferDescPtr;
typedef std::shared_ptr<class ShaderProgram> ShaderProgramPtr;
typedef std::shared_ptr<class Buffer> BufferPtr;

class MR_API UniformBufferDesc final {
public:
    struct Uniform {
        uint32_t index = 0;
        int32_t offset = 0;
        std::string name = "";
    };

    inline int32_t GetSize() const;
    inline ShaderProgramPtr GetProgram() const;
    inline uint32_t GetIndex() const;
    inline std::string GetName() const;
    inline int32_t GetUniformsNum() const;
    inline Uniform GetUniform(int32_t arrayIndex) const;
    inline bool FindUniformByIndex(uint32_t uniformIndex, Uniform& out_uniform) const;
    inline bool FindUniformByName(std::string const& uniformName, Uniform& out_uniform) const;

    static UniformBufferDescPtr Create(ShaderProgramPtr const& program, uint32_t ubo_index);
    static bool Create(ShaderProgramPtr const& program, std::vector<UniformBufferDescPtr>& out_ubos);
private:
    UniformBufferDesc() = default;
    uint32_t _index;
    int32_t _size;
    std::string _name;
    ShaderProgramPtr _program;

    struct sUniforms {
        int32_t num = 0;
        Uniform* arr = nullptr;

        void Resize(int32_t num);
        void Free();
        ~sUniforms();
    };

    sUniforms _uniforms;
};

class MR_API UniformBuffer final {
public:
    inline UniformBufferDescPtr GetDesc() const;
    inline BufferPtr GetBuffer() const;

    template<typename T>
    inline T* As(int32_t arrayIndex);
    void* At(int32_t arrayIndex);

    static UniformBufferPtr Create(UniformBufferDescPtr const& desc);
private:
    bool _ResetBuffer();
    UniformBuffer() = default;
    UniformBufferDescPtr _desc;
    BufferPtr _buffer;
};

inline int32_t UniformBufferDesc::GetSize() const {
    return _size;
}

inline ShaderProgramPtr UniformBufferDesc::GetProgram() const {
    return _program;
}

inline uint32_t UniformBufferDesc::GetIndex() const {
    return _index;
}

inline std::string UniformBufferDesc::GetName() const {
    return _name;
}

inline int32_t UniformBufferDesc::GetUniformsNum() const {
    return _uniforms.num;
}

inline UniformBufferDesc::Uniform UniformBufferDesc::GetUniform(int32_t arrayIndex) const {
    return _uniforms.arr[arrayIndex];
}

inline bool UniformBufferDesc::FindUniformByIndex(uint32_t uniformIndex, UniformBufferDesc::Uniform& out_uniform) const {
    for(int32_t i = 0, n = _uniforms.num; i < n; ++i) {
        if(_uniforms.arr[i].index == uniformIndex) {
            out_uniform = _uniforms.arr[i];
            return true;
        }
    }
    return false;
}

inline bool UniformBufferDesc::FindUniformByName(std::string const& uniformName, Uniform& out_uniform) const {
    for(int32_t i = 0, n = _uniforms.num; i < n; ++i) {
        if(_uniforms.arr[i].name == uniformName) {
            out_uniform = _uniforms.arr[i];
            return true;
        }
    }
    return false;
}

inline UniformBufferDescPtr UniformBuffer::GetDesc() const {
    return _desc;
}

inline BufferPtr UniformBuffer::GetBuffer() const {
    return _buffer;
}

template<typename T>
inline T* UniformBuffer::As(int32_t uniformIndex) {
    void* mem = At(uniformIndex);
    if(mem == nullptr) return nullptr;
    return (T*)mem;
}

}

#ifdef _GLIBCXX_IOSFWD
namespace std {
MR_API std::ostream& operator << (std::ostream& out, mr::UniformBufferDescPtr const& ubo_desc);
}
#endif
