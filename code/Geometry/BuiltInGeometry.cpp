#include "BuiltInGeometry.hpp"

#include "GeometryManager.hpp"
#include "GeometryFormats.hpp"

namespace mr {

IGeometry* BuiltInGeometry::CreateScreenQuad() {
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 texCoord;
    };

    Vertex vertexes[] {
        Vertex {glm::vec3(0,0,0), glm::vec2(0,0)},
        Vertex {glm::vec3(1,0,0), glm::vec2(1,0)},
        Vertex {glm::vec3(1,1,0), glm::vec2(1,1)},
        Vertex {glm::vec3(0,1,0), glm::vec2(0,1)}
    };

    unsigned int indecies[] {
        2, 3, 0,
        0, 1, 2
    };

    GeomDataTypePtr float_DataType = std::make_shared<GeomDataType>(GeomDataType::Float, sizeof(float));
    GeomDataTypePtr ushort_DataType = std::make_shared<GeomDataType>(GeomDataType::UInt, sizeof(unsigned int));

    VertexFormatPtr vertexFormatPtr = std::make_shared<VertexFormat>();
    vertexFormatPtr->attributes = mu::ArrayHandle<VertexAttribute>(new VertexAttribute[2], 2, true, false);
    vertexFormatPtr->pointers = mu::ArrayHandle<uint64_t>(new uint64_t[2], 2, true, false);
    vertexFormatPtr->attribsNum = 2;

    vertexFormatPtr->SetAttribute(std::make_shared<VertexAttributeDesc>(3, MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION, 0, float_DataType), 0);
    vertexFormatPtr->SetAttribute(std::make_shared<VertexAttributeDesc>(2, MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION, 0, float_DataType), 1);

    vertexFormatPtr->Complete();

    IndexFormatPtr indexFormatPtr = std::make_shared<IndexFormat>(ushort_DataType);

    return GeometryManager::GetInstance()->PlaceGeometry(vertexFormatPtr, &vertexes[0].pos.x, 4, indexFormatPtr, &indecies[0], 6, BufferUsage::Static, DrawMode::Triangles);
}

}
