#include "GeometryFormats.hpp"

#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Containers.hpp"

#define GLEW_STATIC
#include <GL\glew.h>

namespace mr {

GeomDataType::GeomDataType() : dataTypeGL(GeomDataType::Float), size(sizeof(float)) {}
GeomDataType::GeomDataType(unsigned int const& dataTypeGL_, unsigned int const& size_)
 : dataTypeGL(dataTypeGL_), size(size_) {}

VertexAttributeDesc::VertexAttributeDesc() : elementsNum(0), size(0), shaderIndex(0), divisor(0), dataType(nullptr) {
}

VertexAttributeDesc::VertexAttributeDesc(unsigned int const& elementsNum_, unsigned int const& size_, unsigned int const& shaderIndex_, unsigned int const& divisor_, GeomDataTypePtr const& dataType_)
: elementsNum(elementsNum_), size(size_), shaderIndex(shaderIndex_), divisor(divisor_), dataType(dataType_) {
}

void VertexFormat::Complete() {
    size = 0;
    attribsNum = attributes.GetNum();
    VertexAttribute* attributesArray = attributes.GetArray();
    for(size_t i = 0; i < attribsNum; ++i) {
        attributesArray[i].offset = CalcOffset(attributesArray, i);
        pointers.GetArray()[i] = (uint64_t)size;
        size += attributesArray[i].desc->size;
    }
}

IndexFormat::IndexFormat() : dataType(nullptr) {}
IndexFormat::IndexFormat(GeomDataTypePtr const& dataType_) : dataType(dataType_) {}

}
