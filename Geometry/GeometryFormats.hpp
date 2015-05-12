#pragma once

#ifndef _MR_GEOMETRY_FORMATS_H_
#define _MR_GEOMETRY_FORMATS_H_

#include "../Types.hpp"
#include "../Config.hpp"
#include "GeometryInterfaces.hpp"
#include <mu/Singleton.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string.h>

namespace mr {

struct GeomDataType;
struct VertexAttributeDesc;
struct VertexAttribute;
struct VertexFormat;
struct IndexFormat;

typedef std::shared_ptr<GeomDataType> GeomDataTypePtr;
typedef std::shared_ptr<VertexAttributeDesc> VertexAttributeDescPtr;
typedef std::shared_ptr<VertexAttribute> VertexAttributePtr;
typedef std::shared_ptr<VertexFormat> VertexFormatPtr;
typedef std::shared_ptr<IndexFormat> IndexFormatPtr;

struct GeomDataType {
    enum DefaultDataType {
        Int = 0x1404,
        UInt = 0x1405,
        Float = 0x1406
    };

    unsigned int dataTypeGL;
    unsigned int size;

    inline bool Equal(GeomDataType const& other) const {
        return !((bool)memcmp(this, &other, sizeof(GeomDataType)));
    }

    GeomDataTypePtr Cache();

    GeomDataType();
    GeomDataType(unsigned int const& dataTypeGL, unsigned int const& size);
};

struct VertexAttributeDesc {
    enum DefaultShaderIndex {
        PositionIndex = MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION,
        NormalIndex = MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION,
        ColorIndex = MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION,
        TexCoordIndex = MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION
    };

    unsigned int elementsNum;
    unsigned int size;
    unsigned int shaderIndex;
    unsigned int divisor;
    GeomDataTypePtr dataType; //should be at the end or rewrite Equal method.

    inline bool Equal(VertexAttributeDesc const& other) const {
        bool dt = (dataType.get() == other.dataType.get());
        if(!dt && (dataType.get() != nullptr && other.dataType.get() != nullptr)) {
            dt = dataType.get()->Equal(*(other.dataType.get()));
        }
        return dt && !((bool)memcmp(this, &other, sizeof(VertexAttributeDesc) - sizeof(GeomDataTypePtr)));
    }

    VertexAttributeDescPtr Cache();

    VertexAttributeDesc();
    VertexAttributeDesc(unsigned int const& elementsNum, unsigned int const& size, unsigned int const& shaderIndex, unsigned int const& divisor, GeomDataTypePtr const& dataType);
};

struct VertexAttribute {
    unsigned int offset;
    VertexAttributeDescPtr desc;

    inline bool Equal(VertexAttribute const& other) const {
        bool bDesc = (desc.get() == other.desc.get());
        if(!bDesc && (desc.get() != nullptr && other.desc.get() != nullptr)) {
            bDesc = desc.get()->Equal(*(other.desc.get()));
        }
        return (offset == other.offset) && bDesc;
    }

    inline bool operator < (VertexAttribute const& b) const {
        return (!Equal(b));
    }
};

struct VertexFormat {
    mu::ArrayHandle<VertexAttribute> attributes;
    mu::ArrayHandle<uint64_t> pointers;
    unsigned int size;
    size_t attribsNum;

    void Complete();
    VertexFormatPtr Cache();

    inline bool Equal(const VertexFormat* format) const {
        if( format == nullptr ||
            attribsNum == 0 ||
            format->attribsNum == 0 ||
            size == 0 || format->size == 0 ||
            !(size == format->size && attribsNum == format->attribsNum)) return false;
        for(size_t i = 0; i < attribsNum; ++i) {
            if(!attributes.GetArray()[i].Equal(format->attributes.GetArray()[i])) return false;
        }
        return true;
    }

    inline void SetAttribute(VertexAttributeDescPtr const& desc, unsigned int const& index) {
        attributes.GetArray()[index].desc = desc;
        attributes.GetArray()[index].offset = CalcOffset(index);
    }

    inline unsigned int CalcOffset(unsigned int const& index) const {
        unsigned int offset = 0;
        for(size_t i = 0; i < index; ++i) {
            if(attributes.GetArray()[i].desc != nullptr)
                offset += attributes.GetArray()[i].desc->size;
        }
        return offset;
    }
};

struct IndexFormat {
    GeomDataTypePtr dataType;

    IndexFormatPtr Cache();

    inline bool Equal(const IndexFormat* format) const {
        if(dataType.get() == format->dataType.get()) return true;
        if(dataType.get() == nullptr) return false;
        if(format->dataType.get() == nullptr) return false;
        return dataType->Equal(*(format->dataType.get()));
    }

    IndexFormat();
    IndexFormat(GeomDataTypePtr const& dataType);
};

}

#endif // _MR_GEOMETRY_FORMATS_H_
