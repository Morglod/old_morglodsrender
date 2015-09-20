#include "GeometryFormats.hpp"

#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Containers.hpp"

#include <GL\glew.h>

namespace mr {

GeomDataType::GeomDataType() : dataTypeGL(GeomDataType::Float), size(sizeof(float)) {}
GeomDataType::GeomDataType(unsigned int const& dataTypeGL_, unsigned int const& size_)
 : dataTypeGL(dataTypeGL_), size(size_) {}

VertexAttributeDesc::VertexAttributeDesc() : elementsNum(0), size(0), shaderIndex(0), divisor(0), dataType(nullptr) {
}

VertexAttributeDesc::VertexAttributeDesc(unsigned int const& elementsNum_, unsigned int const& shaderIndex_, unsigned int const& divisor_, GeomDataTypePtr const& dataType_)
: elementsNum(elementsNum_), size(elementsNum_ * dataType_->size), shaderIndex(shaderIndex_), divisor(divisor_), dataType(dataType_) {
}

VertexAttribute::VertexAttribute() : desc(nullptr) {}
VertexAttribute::VertexAttribute(VertexAttributeDescPtr const& desc_) : desc(desc_) {}

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

VertexFormatPtr VertexFormat::Create(std::initializer_list<VertexAttribute> const& attribs) {
    return VertexFormat::Create(mu::ArrayHandle<VertexAttribute>(attribs));
}

VertexFormatPtr VertexFormat::Create(mu::ArrayHandle<VertexAttribute> const& attribs) {
    const size_t num = attribs.GetNum();
    if(num == 0) return nullptr;
    VertexFormat* vf = new VertexFormat();
    vf->attributes = attribs;
    vf->pointers = mu::ArrayHandle<uint64_t>(new uint64_t[num], num);
    vf->Complete();
    return VertexFormatPtr(vf);
}

bool VertexFormat::GetActiveAttribs(unsigned int programHandle, std::vector<VertexShaderAttribute>& outAttribs) {
    int activeAttributes = 0;
    glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &activeAttributes);
    if(activeAttributes == 0) {
        mr::Log::LogString("No active attributes, for \""+std::to_string(programHandle)+"\" shader program.", MR_LOG_LEVEL_WARNING);
        return true;
    }

    const int nameBufSize = 1024;
    char nameBuf[nameBufSize];
    int nameLen = 0;
    VertexShaderAttribute attrib;

    for(int i = 0; i < activeAttributes; ++i) {
        glGetActiveAttrib(programHandle, i, nameBufSize, &nameLen, &attrib.size, &attrib.typeGL, nameBuf);
        attrib.location = glGetAttribLocation(programHandle, nameBuf);
        attrib.i = i;
        attrib.name = std::string(nameBuf, nameLen);
        outAttribs.push_back(attrib);
    }

    return true;
}

IndexFormat::IndexFormat() : dataType(nullptr) {}
IndexFormat::IndexFormat(GeomDataTypePtr const& dataType_) : dataType(dataType_) {}

}
