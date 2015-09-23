#pragma once

#include "build.hpp"

#include <inttypes.h>
#include <vector>
#include <memory>
#include <future>

namespace mr {

enum class PosDataType : uint32_t {
    Float = 0x1406,
    HalfFloat = 0x140B,
    Int = 0x1404,
    Short = 0x1402
};

enum class ColorDataType : uint32_t {
    UByte = 0x1401,
    UShort = 0x1403,
    UInt = 0x1405,
    HalfFloat = 0x140B,
    Float = 0x1406
};

typedef std::shared_ptr<class VertexDecl> VertexDeclPtr;

/**
    Align all attribs size to 4 or it's datatype size
**/

class MR_API VertexDecl final {
    friend class VertexBuffer;
public:
    struct Attrib {
        uint8_t bindpoint;
        uint8_t components_num;
        uint32_t datatype;
        uint8_t normalized;
        uint8_t offset;
    };

    struct MR_API AttribArray {
        Attrib* attribs = nullptr;
        uint8_t num = 0;

        void Resize(uint8_t n);
        void Free();

        AttribArray() = default;
        ~AttribArray();
    };

    class MR_API Changer final {
        friend class VertexDecl;
    public:
        struct AttribDesc {
            uint32_t gl_data_type;
            uint8_t components_num;
            uint8_t offset;
            uint8_t bindpoint;
            uint8_t size;
            bool normalized;
        };

        Changer& Pos(PosDataType const& type = PosDataType::HalfFloat); // vec3<DataType>
        Changer& Color(ColorDataType const& type = ColorDataType::UByte); // vec4<DataType>
        Changer& Data(uint8_t sz); // skip data block, sizeof 'sz'
        void End();

    protected:
        Changer(VertexDecl& d);
        void Push(uint32_t gl_dt, uint8_t comp_num, bool norm);

        VertexDecl& decl;
        uint32_t offset = 0;
        uint8_t attribi = 0;
        std::vector<AttribDesc> attribs;
    };

    Changer Begin();
    std::future<bool> Bind(uint32_t binding);
    VertexDecl() = default;

    inline uint32_t GetSize() const { return _size; }

    inline static VertexDeclPtr Create() {
        return VertexDeclPtr(new VertexDecl());
    }

protected:
    static bool _Bind(VertexDecl* decl, uint32_t binding);

private:
    AttribArray _attribs;
    uint32_t _size = 0;
};

}
