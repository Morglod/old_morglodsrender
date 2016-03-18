#pragma once

#include "mr/build.hpp"
#include "mr/pre/glm.hpp"
#include "mr/string.hpp"
#include "mr/log.hpp"

#include <memory>
#include <vector>
#include <unordered_map>

namespace mr {

typedef std::shared_ptr<class UniformBuffer> UniformBufferPtr;
typedef std::shared_ptr<class UniformBufferDecl> UniformBufferDeclPtr;
typedef std::shared_ptr<class ShaderProgram> ShaderProgramPtr;
typedef std::shared_ptr<class Buffer> BufferPtr;

class MR_API UniformBufferDecl final {
    friend class _Alloc;
public:
    struct Uniform {
        int32_t offset = 0;
        std::string name = "";
        int32_t size = 0;

        inline bool IsGood() const {
            return size != 0;
        }
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

struct MR_API UniformRefAny {
    void* ptr;
    int32_t size;
    inline UniformRefAny() : ptr(nullptr), size(0) {}
    inline UniformRefAny(void* v, int32_t s) : ptr(v), size(s) {}
    inline bool IsGood() const { return (ptr != nullptr) && (size != 0); }
};

template<typename T>
struct MR_API UniformRef : public UniformRefAny {
    inline T Get() const {
        return *((T*)ptr);
    }

    inline T& Value() {
        return *((T*)ptr);
    }

    inline T operator = (T const& value) {
        return ((*((T*)ptr)) = value);
    }

    inline UniformRef() : UniformRefAny() {}
    inline UniformRef(T* t) : UniformRefAny(t, sizeof(T)) {}
    inline UniformRef(UniformRefAny const& any) : UniformRefAny(any.ptr, any.size) {}
};

class MR_API UniformBuffer final {
    friend class _Alloc;
public:
    inline UniformBufferDeclPtr GetDecl() const;
    inline BufferPtr GetBuffer() const;
    bool SetBuffer(BufferPtr const& buffer);
    inline bool Match(UniformBufferDeclPtr const& other) const;

    template<typename T>
    inline UniformRef<T> As(int32_t arrayIndex);

    template<typename T>
    inline UniformRef<T> As(std::string const& name);

    UniformRefAny At(int32_t arrayIndex) const;
    UniformRefAny At(std::string const& name) const;

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
    if(arrayIndex >= _uniforms.num || arrayIndex < 0) {
        MR_LOG_ERROR(UniformBufferDecl::GetUniform, "Invalid index");
        return UniformBufferDecl::Uniform();
    }
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

/// UniformBuffer::Ref

template<typename T>
inline UniformRef<T> UniformBuffer::As(int32_t arrayIndex) {
    return UniformRef<T>(At(arrayIndex));
}

template<typename T>
inline UniformRef<T> UniformBuffer::As(std::string const& name) {
    return UniformRef<T>(At(name));
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

}

#ifdef _GLIBCXX_IOSFWD
namespace std {
MR_API std::ostream& operator << (std::ostream& out, mr::UniformBufferDeclPtr const& ubo_desc);
}
#endif
