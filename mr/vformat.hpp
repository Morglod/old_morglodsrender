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
        uint8_t normalized; // 0 or 1
        uint8_t offset;
        uint8_t size;
    };

    struct MR_API BindPoint {
        /// Do not touch this order, in VertexDecl::Equal memcmp used
        uint8_t bindpoint = 0;
        uint8_t stride = 0;
        uint8_t num = 0;

        Attrib* attribs = nullptr;

        void Free();
        void Resize(uint8_t n);

        BindPoint() = default;
        ~BindPoint();
    };

    struct MR_API AttribMap {
        uint8_t num = 0;
        BindPoint* bindpoints = nullptr;

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
        inline Changer& Normal(PosDataType const& type = PosDataType::Float, uint8_t bindpoint = 0); // vec3<DataType>
        Changer& Data(uint8_t sz, uint8_t bindpoint = 0); // skip data block, sizeof 'sz'
        Changer& Custom(DataType const& type, uint8_t components_num, uint8_t bindpoint = 0, bool normalized = false);
        void End();

    protected:
        Changer(VertexDecl& d);
        void Push(uint8_t bindpoint, uint32_t gl_dt, uint8_t comp_num, bool norm, bool offsetOnly);

        VertexDecl& decl;
        std::unordered_map<uint8_t, BindPointDesc> bindpoints; // [binding point].attributes[]
    };

    Changer Begin();
    bool Bind();

    inline uint32_t GetSize() const;
    inline uint8_t GetBindpointsNum() const;
    inline uint8_t GetBindpointIndex(uint8_t arrayIndex) const;
    inline uint8_t GetBindpointStride(uint8_t arrayIndex) const;
    inline uint8_t GetBindpointAttribsNum(uint8_t arrayIndex) const;
    inline Attrib GetAttribute(uint8_t bindpointArrayIndex, uint8_t attribArrayIndex) const;

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

inline uint8_t VertexDecl::GetBindpointsNum() const {
    return _map.num;
}

inline uint8_t VertexDecl::GetBindpointIndex(uint8_t arrayIndex) const {
    return _map.bindpoints[arrayIndex].bindpoint;
}

inline uint8_t VertexDecl::GetBindpointAttribsNum(uint8_t arrayIndex) const {
    return _map.bindpoints[arrayIndex].num;
}

inline VertexDecl::Attrib VertexDecl::GetAttribute(uint8_t bindpointArrayIndex, uint8_t attribArrayIndex) const {
    return _map.bindpoints[bindpointArrayIndex].attribs[attribArrayIndex];
}

inline VertexDecl::Changer& VertexDecl::Changer::Normal(PosDataType const& type, uint8_t bindpoint) {
    return Pos(type, bindpoint);
}

inline bool VertexDecl::Equal(const VertexDecl* other) const {
    if(_size != other->_size) return false;
    if(_map.num != other->_map.num) return false;
    for(uint8_t ibp = 0, nbp = _map.num; ibp < nbp; ++ibp) {
        if(memcmp(&_map.bindpoints[ibp], &(other->_map.bindpoints[ibp]), offsetof(VertexDecl::BindPoint, attribs)) != 0) return false;
        const Attrib* attribs = _map.bindpoints[ibp].attribs;
        const Attrib* otherAttribs = other->_map.bindpoints[ibp].attribs;
        const uint8_t attribsNum = other->_map.bindpoints[ibp].num;
        for(uint8_t i = 0; i < attribsNum; ++i) {
            if(memcmp(&attribs[i], &otherAttribs[i], sizeof(Attrib)) != 0) return false;
        }
    }
    return true;
}

}
