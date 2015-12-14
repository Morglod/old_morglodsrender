#pragma once

#include "../build.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace mr {

typedef std::shared_ptr<class IUniformCacheValue> IUniformCacheValuePtr;
class ShaderProgram;

class MR_API IUniformCacheValue {
public:
    std::string name;

    template<typename T>
    static void Update(const ShaderProgram* shaderProgram, std::string const& name, int32_t num, const T* data);

    // Update value of this shaderProgram
    virtual void Update(const ShaderProgram* shaderProgram) = 0;

    // Set current value
    virtual void Set(const void* valuePtr) = 0;

    // Get value (copy to 'to')
    virtual void Get(void* to) = 0;

    // Get size of value's type
    virtual uint16_t GetSize() = 0;

    inline IUniformCacheValue(std::string const& name_) : name(name_) {}
    inline virtual ~IUniformCacheValue() {}
};

template<typename T, int TVecSize>
class UniformCacheValue : public IUniformCacheValue {
public:
    static_assert(TVecSize <= 4, "vec<4> is max");
    T value[TVecSize];

    inline void Update(const ShaderProgram* shaderProgram) override {
        IUniformCacheValue::Update<T>(shaderProgram, name, TVecSize, &value[0]);
    }

    inline void Set(const void* valuePtr) override { memcpy(&value[0], valuePtr, sizeof(T)*TVecSize); }
    inline void Get(void* to) override { memcpy(to, &value[0], sizeof(T)*TVecSize); }
    inline uint16_t GetSize() override { return sizeof(T); }

    inline UniformCacheValue(std::string const& name_) : IUniformCacheValue(name_) {}
    inline virtual ~UniformCacheValue() {}
};

class MR_API UniformCache final {
public:
    static UniformCache* Get();

    template<typename T, int TVecSize>
    inline void Set(std::string const& name, const T* value) {
        auto cacheValue = Get(name);
        if(cacheValue == nullptr) {
            cacheValue = IUniformCacheValuePtr(static_cast<IUniformCacheValue*>(new UniformCacheValue<T, TVecSize>(name)));
            _uniforms[name] = cacheValue;
        }
        cacheValue->Set(value);
        _Update(cacheValue.get());
    }

    template<typename T, int TVecSize>
    inline void Set(std::string const& name, const T value) {
        Set<T, TVecSize>(name, &value);
    }

    inline IUniformCacheValuePtr Get(std::string const& name) {
        auto it = _uniforms.find(name);
        if(_uniforms.end() == it) return nullptr;
        return it->second;
    }
private:
    void _Update(IUniformCacheValue* value);
    std::unordered_map<std::string, IUniformCacheValuePtr> _uniforms;
};

}
