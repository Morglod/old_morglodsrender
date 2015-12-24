#pragma once

#include "build.hpp"
#include "gl/types.hpp"

#include <inttypes.h>
#include <vector>
#include <memory>
#include <future>
#include <unordered_map>

namespace mr {

constexpr uint8_t ShaderLocation_Pos = 0;
constexpr uint8_t ShaderLocation_TexCoord = 1;
constexpr uint8_t ShaderLocation_Normal = 2;
constexpr uint8_t ShaderLocation_Color = 3;

typedef std::shared_ptr<class VertexDecl> VertexDeclPtr;

/**
    Align all attribs size to 4 or it's datatype size
**/

class MR_API VertexDecl final {
    friend class VertexBuffer;
public:
    struct Attrib {
        uint8_t location; // shader location
        uint8_t components_num;
        uint32_t datatype;
        uint8_t normalized; // 0 or 1
        uint8_t offset;
        uint8_t size;
        uint8_t stride;
        uint8_t bindpoint;
    };

    struct MR_API AttribMap {
        uint8_t num = 0;
        Attrib* attribs = nullptr;

        void Resize(uint8_t n);
        void Free();

        AttribMap() = default;
        ~AttribMap();
    };

    class MR_API Changer final {
        friend class VertexDecl;
    public:
        struct BindPointDesc {
            uint32_t offset = 0;
            uint8_t attribi = 0;
            std::vector<Attrib> attribs;
        };

        Changer& Pos(PosDataType const& type = PosDataType::Float, uint8_t bindpoint = 0); // vec3<DataType>
        Changer& Color(ColorDataType const& type = ColorDataType::UByte, uint8_t bindpoint = 0); // vec4<DataType>
        Changer& Normal(PosDataType const& type = PosDataType::Float, uint8_t bindpoint = 0); // vec3<DataType>
        Changer& TexCoord(uint8_t bindpoint = 0); // vec2<float>
        Changer& Data(uint8_t sz, uint8_t bindpoint = 0); // skip data block, sizeof 'sz'
        Changer& Custom(DataType const& type, uint8_t components_num, uint8_t shaderLocation, uint8_t bindpoint = 0, bool normalized = false);
        void End();

    protected:
        Changer(VertexDecl& d);
        void Push(uint8_t bindpoint, uint32_t gl_dt, uint8_t comp_num, bool norm, bool offsetOnly, uint8_t shaderLocation);

        VertexDecl& decl;
        std::unordered_map<uint8_t, BindPointDesc> bindpoints; // [binding point].attributes[]
    };

    Changer Begin();
    bool Bind();

    inline uint32_t GetSize() const;
    inline uint8_t GetAttribsNum() const;
    inline Attrib GetAttribute(uint8_t attribArrayIndex) const;

    inline bool Equal(const VertexDecl* other) const;

    static VertexDeclPtr Create();

private:
    VertexDecl() = default;
    AttribMap _map;
    uint32_t _size = 0; // total vertex size
};

inline uint32_t VertexDecl::GetSize() const {
    return _size;
}

inline uint8_t VertexDecl::GetAttribsNum() const {
    return _map.num;
}

inline VertexDecl::Attrib VertexDecl::GetAttribute(uint8_t attribArrayIndex) const {
    return _map.attribs[attribArrayIndex];
}

inline bool VertexDecl::Equal(const VertexDecl* other) const {
    if(_size != other->_size) return false;
    if(_map.num != other->_map.num) return false;
    return (memcmp(_map.attribs, other->_map.attribs, sizeof(Attrib)*_map.num) == 0);
}

}
