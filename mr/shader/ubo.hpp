#pragma once

#include "mr/build.hpp"
#include "mr/pre/glm.hpp"
#include "mr/string.hpp"

#include <memory>
#include <vector>
#include <unordered_map>

#define _MR_TO_STRING(x) (#x)

#define _MR_DEF_SYS_UNIFORM(x) \
struct SysUniformData { \
    x \
}; \
const std::string SysUniformStr = "uniform "+SysUniformNameBlock+" { " +ReplaceString((#x), "glm::", "")+ " } "+SysUniformName+"; \n";

namespace mr {

typedef std::shared_ptr<class UniformBuffer> UniformBufferPtr;
typedef std::shared_ptr<class UniformBufferDecl> UniformBufferDeclPtr;
typedef std::shared_ptr<class ShaderProgram> ShaderProgramPtr;
typedef std::shared_ptr<class Buffer> BufferPtr;

/// const std::string SysUniformStr;
const std::string SysUniformName = "mr_sys";
const std::string SysUniformNameBlock = SysUniformName+"_block";

_MR_DEF_SYS_UNIFORM(
	glm::mat4 view;
	glm::mat4 proj;
	float time;
)

class MR_API UniformBufferDecl final {
public:
    struct Uniform {
        int32_t offset = 0;
        std::string name = "";
    };

    inline int32_t GetSize() const;
    inline int32_t GetUniformsNum() const;
    inline Uniform GetUniform(int32_t arrayIndex) const;
    inline bool FindUniformByName(std::string const& uniformName, Uniform& out_uniform, int32_t& out_arrayIndex) const;
    inline bool Equal(UniformBufferDeclPtr const& other) const;

    static UniformBufferDeclPtr Create(ShaderProgram* program, uint32_t ubo_index);
    static UniformBufferDeclPtr Create(ShaderProgram* program, std::string const& ubo_name);
    static bool Create(ShaderProgram* program, std::vector<std::pair<std::string, UniformBufferDeclPtr>>& out_ubos); // <name, decl>

    inline static UniformBufferDeclPtr Create(ShaderProgramPtr const& program, uint32_t ubo_index);
    inline static UniformBufferDeclPtr Create(ShaderProgramPtr const& program, std::string const& ubo_name);
    inline static bool Create(ShaderProgramPtr const& program, std::vector<std::pair<std::string, UniformBufferDeclPtr>>& out_ubos); /** out_ubos is <name, decl> **/
private:
    UniformBufferDecl() = default;
    int32_t _size;

    struct sUniforms {
        int32_t num = 0;
        Uniform* arr = nullptr;

        void Resize(int32_t num);
        void Free();
        ~sUniforms();
    };

    sUniforms _uniforms;
};


template<typename T>
struct MR_API UniformRef {
    T* ptr;

    inline T Get() const {
        return *ptr;
    }

    inline T& Value() {
        return *ptr;
    }

    inline T operator = (T const& value) {
        return ((*ptr) = value);
    }
};

class MR_API UniformBuffer final {
public:
    inline UniformBufferDeclPtr GetDecl() const;
    inline BufferPtr GetBuffer() const;
    bool SetBuffer(BufferPtr const& buffer);
    inline bool Match(UniformBufferDeclPtr const& other) const;

    template<typename T>
    inline T* AsPtr(int32_t arrayIndex);

    template<typename T>
    inline T* AsPtr(std::string const& name);

    template<typename T>
    inline T& As(int32_t arrayIndex);

    template<typename T>
    inline T& As(std::string const& name);

    void* At(int32_t arrayIndex);
    void* At(std::string const& name);

    template<typename T>
    inline UniformRef<T> Ref(int32_t arrayIndex);

    template<typename T>
    inline UniformRef<T> Ref(std::string const& name);

    static UniformBufferPtr Create(UniformBufferDeclPtr const& desc);
    static UniformBufferPtr Create(UniformBufferDeclPtr const& desc, BufferPtr const& buffer);
private:
    bool _ResetBuffer();
    UniformBuffer() = default;
    UniformBufferDeclPtr _desc;
    BufferPtr _buffer;
};

/// UniformBufferDecl inline

inline int32_t UniformBufferDecl::GetSize() const {
    return _size;
}

inline int32_t UniformBufferDecl::GetUniformsNum() const {
    return _uniforms.num;
}

inline UniformBufferDecl::Uniform UniformBufferDecl::GetUniform(int32_t arrayIndex) const {
    return _uniforms.arr[arrayIndex];
}

inline bool UniformBufferDecl::FindUniformByName(std::string const& uniformName, Uniform& out_uniform, int32_t& out_arrayIndex) const {
    for(int32_t i = 0, n = _uniforms.num; i < n; ++i) {
        if(_uniforms.arr[i].name == uniformName) {
            out_uniform = _uniforms.arr[i];
            out_arrayIndex = i;
            return true;
        }
    }
    return false;
}

inline UniformBufferDeclPtr UniformBufferDecl::Create(ShaderProgramPtr const& program, uint32_t ubo_index) {
    return Create(program.get(), ubo_index);
}

inline UniformBufferDeclPtr UniformBufferDecl::Create(ShaderProgramPtr const& program, std::string const& ubo_name) {
    return Create(program.get(), ubo_name);
}

/** out_ubos is <name, decl> **/
inline bool UniformBufferDecl::Create(ShaderProgramPtr const& program, std::vector<std::pair<std::string, UniformBufferDeclPtr>>& out_ubos) {
    return UniformBufferDecl::Create(program.get(), out_ubos);
}

inline bool UniformBufferDecl::Equal(UniformBufferDeclPtr const& other) const {
    if(_size != other->_size) return false;
    if(_uniforms.num != other->_uniforms.num) return false;
    for(int32_t i = 0, n = _uniforms.num; i < n; ++i) {
        if(_uniforms.arr[i].name !=  other->_uniforms.arr[i].name) return false;
        if(_uniforms.arr[i].offset !=  other->_uniforms.arr[i].offset) return false;
    }
    return true;
}

/// UniformBuffer::Ref inline

template<typename T>
inline UniformRef<T> UniformBuffer::Ref(int32_t arrayIndex) {
    T* t = (T*)At(arrayIndex);
    return UniformRef<T>{t};
}

template<typename T>
inline UniformRef<T> UniformBuffer::Ref(std::string const& name) {
    T* t = (T*)At(name);
    return UniformRef<T>{t};
}

/// UniformBuffer inline

inline bool UniformBuffer::Match(UniformBufferDeclPtr const& other) const {
    return _desc->Equal(other);
}

inline UniformBufferDeclPtr UniformBuffer::GetDecl() const {
    return _desc;
}

inline BufferPtr UniformBuffer::GetBuffer() const {
    return _buffer;
}

template<typename T>
inline T* UniformBuffer::AsPtr(int32_t arrayIndex) {
    void* mem = At(arrayIndex);
    if(mem == nullptr) return nullptr;
    return (T*)mem;
}

template<typename T>
inline T* UniformBuffer::AsPtr(std::string const& name) {
    void* mem = At(name);
    if(mem == nullptr) return nullptr;
    return (T*)mem;
}

template<typename T>
inline T& UniformBuffer::As(int32_t arrayIndex) {
    void* mem = At(arrayIndex);
    if(mem == nullptr) return nullptr;
    return *((T*)mem);
}

template<typename T>
inline T& UniformBuffer::As(std::string const& name) {
    void* mem = At(name);
    return *((T*)mem);
}

}

#ifdef _GLIBCXX_IOSFWD
namespace std {
MR_API std::ostream& operator << (std::ostream& out, mr::UniformBufferDeclPtr const& ubo_desc);
}
#endif

#undef _MR_DEF_SYS_UNIFORM
#undef _MR_TO_STRING
