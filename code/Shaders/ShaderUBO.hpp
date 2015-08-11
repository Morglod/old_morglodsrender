#pragma once

#include "../CoreObjects.hpp"

#include <unordered_map>
#include <memory>

namespace mr {

class IBuffer;

struct ShaderUBOParams {
public:
    struct Named { size_t size; std::string name; };
    struct Desc { size_t size; size_t offset; };

    std::unordered_map<std::string, Desc> params;

    inline size_t CalcTotalSize() const {
        size_t sz = 0;
        for(std::pair<std::string, Desc> const& p : params) sz += p.second.size;
        return sz;
    }
};

class ShaderUBO : public IGPUObjectHandle {
public:
    virtual bool CreateBuffer(size_t const& size, bool dynamic = false);
    virtual bool SetData(void* data, size_t const& size, bool dynamic = false);

    unsigned int GetGPUHandle() override;
    size_t GetGPUMem() override;
    void Destroy() override;

    inline ShaderUBOParams& GetParams() { return _params; }


    /** ALL PARAMS SHOULD FIT IN CREATED BUFFER SIZE */

    virtual bool SetParam(std::string const& name, void* value);

    virtual bool AddParam(std::string const& name, size_t const& size, size_t const& offset, void* value = nullptr);
    virtual bool AddParam(std::string const& name, ShaderUBOParams::Desc const& desc, void* value = nullptr);
    virtual bool AddParams(std::vector<ShaderUBOParams::Named> const& params);
    virtual bool SetParams(std::vector<ShaderUBOParams::Named> const& params);

    ShaderUBO();
    virtual ~ShaderUBO();
protected:
    IBuffer* _buffer = nullptr;
    ShaderUBOParams _params;
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
