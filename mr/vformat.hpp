#pragma once

#include "build.hpp"
#include "gl/types.hpp"

#include <inttypes.h>
#include <vector>
#include <memory>
#include <future>
#include <unordered_map>

namespace mr {

typedef std::shared_ptr<class VertexDecl> VertexDeclPtr;

/**
    Align all attribs size to 4 or it's datatype size
**/

class MR_API VertexDecl final {
    friend class VertexBuffer;
public:
    struct Attrib {
        uint8_t index; // attrib shader index
        uint8_t components_num;
        uint32_t datatype;
        uint8_t normalized;
        uint8_t offset;
    };

    struct MR_API BindPoint {
        uint8_t bindpoint = 0;
        uint8_t stride = 0;

        Attrib* attribs = nullptr;
        uint8_t num = 0;

        void Free();
        void Resize(uint8_t n);

        BindPoint() = default;
        ~BindPoint();
    };

    struct MR_API AttribMap {
        BindPoint* bindpoints = nullptr;
        uint8_t num;

        void Resize(uint8_t n);
        void Free();

        AttribMap() = default;
        ~AttribMap();
    };

    class MR_API Changer final {
        friend class VertexDecl;
    public:
        struct AttribDesc {
            uint32_t gl_data_type;
            uint8_t components_num;
            uint8_t offset;
            uint8_t index;
            uint8_t size;
            bool normalized;
        };

        struct BindPointDesc {
            uint32_t offset = 0;
            uint8_t attribi = 0;
            std::vector<AttribDesc> attribs;
        };

        Changer& Pos(PosDataType const& type = PosDataType::Float, uint8_t bindpoint = 0); // vec3<DataType>
        Changer& Color(ColorDataType const& type = ColorDataType::UByte, uint8_t bindpoint = 0); // vec4<DataType>
        Changer& Data(uint8_t sz, uint8_t bindpoint = 0); // skip data block, sizeof 'sz'
        void End();

    protected:
        Changer(VertexDecl& d);
        void Push(uint8_t bindpoint, uint32_t gl_dt, uint8_t comp_num, bool norm, bool offsetOnly);

        VertexDecl& decl;
        std::unordered_map<uint8_t, BindPointDesc> bindpoints; // [binding point].attributes[]
    };

    Changer Begin();
    bool Bind();
    VertexDecl() = default;

    inline uint32_t GetSize() const;

    static VertexDeclPtr Create();

private:
    AttribMap _map;
    uint32_t _size = 0; // total vertex size
};

inline uint32_t VertexDecl::GetSize() const {
    return _size;
}

}
