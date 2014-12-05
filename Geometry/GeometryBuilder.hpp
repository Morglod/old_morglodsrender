#pragma once

#ifndef _MR_GEOMETRY_BUILDER_H_
#define _MR_GEOMETRY_BUILDER_H_

#include "../Utils/Containers.hpp"
#include "../Buffers/BuffersInterfaces.hpp"

#include "../Shaders/ShaderConfig.hpp"

#include <vector>
#include <glm/glm.hpp>

namespace mr {

class IGeometry;
class ManualGeometry {
public:
    typedef unsigned int IndexType;
    const unsigned int GPUIndexType = 0x1405; //GL_UNSIGNED_INT

    enum VertexDataAttributeType {
        Position = MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION,
        Normal = MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION,
        Color = MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION,
        TexCoord = MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION
    };

    struct VertexDataAttribute {
    public:
        unsigned int attributeType; //shader index also
        unsigned int elementsNum;
        VertexDataAttribute();
        VertexDataAttribute(unsigned int const& t, unsigned int const& e);
    };

    /**
        Returns index of first element of added data.
    **/

    IndexType AddVertexData(unsigned int const& attribute, glm::vec4 const& data, unsigned int const& elementsNum);
    inline IndexType AddVertexData(unsigned int const& attribute, float const& data) { return AddVertexData(attribute, glm::vec4(data, 0,0,0), 1); }
    inline IndexType AddVertexData(unsigned int const& attribute, glm::vec2 const& data) { return AddVertexData(attribute, glm::vec4(data, 0.0f, 0.0f), 2); }
    inline IndexType AddVertexData(unsigned int const& attribute, glm::vec3 const& data) { return AddVertexData(attribute, glm::vec4(data, 0.0f), 3); }
    inline IndexType AddVertexData(unsigned int const& attribute, glm::vec4 const& data) { return AddVertexData(attribute, data, 4); }

    IndexType AddIndexData(IndexType const& index);
    inline IndexType AddIndexData(IndexType const& i1, IndexType const& i2, IndexType const& i3) { AddIndexData(i1); AddIndexData(i2); return AddIndexData(i3); }

    IGeometry* Build(IGPUBuffer::Usage const& usage);
    void Clear();

    ManualGeometry();
    ~ManualGeometry();
protected:
    std::vector<float> _vertexData;
    std::vector<IndexType> _indexData;
    size_t _firstVertexAttribute, _lastVertexAttribute, _vertexes;
    std::vector<VertexDataAttribute> _vertexAttributes;
};

}

#endif // _MR_GEOMETRY_BUILDER_H_
