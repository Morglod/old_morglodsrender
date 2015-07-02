#pragma once

#include "../CoreObjects.hpp"

namespace mr {

class IGPUBuffer;

class ShaderUBO : public IGPUObjectHandle {
public:
    virtual bool SetData(void* data, size_t const& size, bool dynamic = false);

    unsigned int GetGPUHandle() override;
    size_t GetGPUMem() override;
    void Destroy() override;

    ShaderUBO();
    virtual ~ShaderUBO();
protected:
    virtual bool _CreateBuffer(size_t const& size, bool dynamic = false);
    IGPUBuffer* _buffer = nullptr;
};

template<typename T>
class TShaderUBO : public ShaderUBO {
public:
    inline size_t GetGPUMem() override {
        return sizeof(T);
    }

    inline bool SetData(T& t) {
        return SetData(&t, sizeof(T));
    }

    virtual ~TShaderUBO() {}
};

}
