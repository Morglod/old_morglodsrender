#include "GeometryBuilder.hpp"
#include "../Utils/Debug.hpp"

#include "GeometryFormats.hpp"
#include "GeometryManager.hpp"

#define MR_MIN(x,y) ((x>y) ? y : x)

namespace mr {

ManualGeometry::VertexDataAttribute::VertexDataAttribute() : attributeType(0), elementsNum(0) {
}

ManualGeometry::VertexDataAttribute::VertexDataAttribute(unsigned int const& t, unsigned int const& e) : attributeType(t), elementsNum(e) {
}

ManualGeometry::IndexType ManualGeometry::AddVertexData(unsigned int const& attribute, glm::vec4 const& data, unsigned int const& elementsNum) {
    Assert(elementsNum != 0);
    Assert(elementsNum <= 4);

    if(_vertexData.size() == 0) {
        _firstVertexAttribute = attribute;
    } else if(attribute == _firstVertexAttribute) {
        ++_vertexes;
    }


    //Find or create attribute

    VertexDataAttribute a;
    bool bAFinded = false;

    for(size_t i = 0; i < _vertexAttributes.size(); ++i){
        if(_vertexAttributes[i].attributeType == attribute) {
            a = _vertexAttributes[i];
            bAFinded = true;
            break;
        }
    }

    if(!bAFinded) {
        a = VertexDataAttribute(attribute, elementsNum);
        _vertexAttributes.push_back(a);
    }

    //Add attribute's data
    ManualGeometry::IndexType index = (ManualGeometry::IndexType)_vertexData.size();

    for(size_t i = 0; i < MR_MIN(a.elementsNum, elementsNum); ++i) {
        _vertexData.push_back(data[i]);
    }

    if(a.elementsNum > elementsNum) {
        for(size_t i = 0; i < (a.elementsNum - elementsNum); ++i) {
            _vertexData.push_back(0.0f);
        }
    }

    return index;
}

ManualGeometry::IndexType ManualGeometry::AddIndexData(IndexType const& index) {
    ManualGeometry::IndexType i = (ManualGeometry::IndexType)_indexData.size();
    _indexData.push_back(index);
    return i;
}

IGeometry* ManualGeometry::Build(IGPUBuffer::Usage const& usage) {
    VertexFormatCustomFixed vformat;
    vformat.SetAttributesNum(_vertexAttributes.size());

    for(size_t i = 0; i < _vertexAttributes.size(); ++i) {
        vformat.AddVertexAttribute(mr::VertexAttributeCustom(_vertexAttributes[i].elementsNum, &(VertexDataTypeFloat::GetInstance()), _vertexAttributes[i].attributeType).Cache());
    }

    IndexFormatCustom iformat(VertexDataTypeCustom(GPUIndexType, sizeof(ManualGeometry::IndexType)).Cache());
    return mr::GeometryManager::GetInstance()->PlaceGeometry(vformat.Cache(), &_vertexData[0], _vertexes+1, iformat.Cache(), &_indexData[0], _indexData.size()+1, usage, IGeometryBuffer::DrawMode::Triangles); //GL_TRIANGLES - 0x0004
}

void ManualGeometry::Clear() {
    _vertexData.clear();
    _indexData.clear();
    _firstVertexAttribute = 0;
    _lastVertexAttribute = 0;
    _vertexes = 0;
    _vertexAttributes.clear();
}

ManualGeometry::ManualGeometry() : _vertexData(), _indexData(), _firstVertexAttribute(0), _lastVertexAttribute(0), _vertexes(0), _vertexAttributes() {
}

ManualGeometry::~ManualGeometry() {
    Clear();
}

}
