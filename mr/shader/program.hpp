#pragma once

#include "mr/build.hpp"
#include "mr/pre/glm.hpp"

#include <memory>
#include <future>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace mr {

typedef std::shared_ptr<class ShaderProgram> ShaderProgramPtr;
typedef std::shared_ptr<class Shader> ShaderPtr;
typedef std::shared_ptr<class UniformBuffer> UniformBufferPtr;
typedef std::shared_ptr<class UniformBufferDecl> UniformBufferDeclPtr;

class MR_API ShaderProgram final {
    friend class Draw;
    friend class _Alloc;
public:
    struct UboInfo {
        std::string name = "";
        UniformBufferPtr ubo = nullptr;
    };

    struct MR_API FoundUniform {
        std::string uniformName;
        UniformBufferPtr ubo;
        int32_t arrayIndex;
        int32_t size;

        void* GetPtr() const;

        template<typename T>
        inline T* AsPtr() {
            return (T*)GetPtr();
        }

        FoundUniform() = default;
    };

    // shaders may be empty
    static ShaderProgramPtr Create(std::vector<ShaderPtr> const& shaders = std::vector<ShaderPtr>());
    void Destroy();

    bool Link(std::vector<ShaderPtr> const& shaders);

    bool SetUniformBuffer(uint32_t arrayIndex, UniformBufferPtr const& ubo);
    bool SetUniformBuffer(std::string const& name, UniformBufferPtr const& ubo);
    inline uint32_t GetUniformBuffersNum() const;
    bool GetUniformBuffer(std::string const& name, ShaderProgram::UboInfo& out_ubo);
    bool GetUniformBuffer(uint32_t arrayIndex, ShaderProgram::UboInfo& out_ubo) const;

    bool FindUniform(std::string const& uniformName, FoundUniform& out_uniform) const;

    inline int32_t GetUniformLocation(std::string const& name) const;

    static void Use(ShaderProgramPtr const& program);
    static std::unordered_set<ShaderProgram*> GetRegisteredPrograms();

    inline uint32_t GetId() const;

    ~ShaderProgram();

protected:
    void _BindUniformBuffer(uint32_t index, uint32_t buffer);

    ShaderProgram();
    bool _ResetUniforms();
    uint32_t _id;

    struct sUBOList {
        UboInfo* arr = nullptr;
        uint32_t num = 0;

        void Resize(uint32_t num);
        void Free();
        ~sUBOList();
    };

    sUBOList _ubo;

    std::unordered_map<std::string, int32_t> _uniformLocations;
};

///

inline uint32_t ShaderProgram::GetId() const {
    return _id;
}

inline uint32_t ShaderProgram::GetUniformBuffersNum() const {
    return _ubo.num;
}

inline int32_t ShaderProgram::GetUniformLocation(std::string const& name) const {
    auto it = _uniformLocations.find(name);
    if(it == _uniformLocations.end()) return -1;
    return it->second;
}

}
