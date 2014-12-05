#pragma once

#ifndef _MR_GEOMETRY_LOADER_H_
#define _MR_GEOMETRY_LOADER_H_

#include <string>
#include <memory>

#include "../Utils/Containers.hpp"

namespace mr {

class IGeometry;

class GeometryLoader final {
public:
    struct sDataHeader {
        unsigned int id; //unique id
    };

    struct sVertexDataType : public sDataHeader {
        unsigned int openglDataType;
        unsigned int size;
    };

    struct sVertexAttribute : public sDataHeader {
        unsigned int vertexDataTypeId;
        unsigned int elementsNum;
        unsigned int shaderIndex;
    };

    struct sVertexFormat : public sDataHeader {
        unsigned int vertexAttribNum;
        //VertexAttributes Ids [vertexAttribNum]
    };

    struct sIndexFormat : public sDataHeader {
        unsigned int vertexDataTypeId;
    };

    struct sGeometryData : public sDataHeader {
        unsigned int vertexFormatId, indexFormatId;
        unsigned int vertexNum, indexNum;
        unsigned int usage, drawMode;
        //binary vertex data sizeof(vertexFormat.Size * vertexNum)
        //binary index data sizeof(indexFormat.Size * indexNum)
    };

    bool Import(std::string const& file);
    unsigned int GetGeometriesNum();

    TStaticArray<IGeometry*> GetGeometry();

    GeometryLoader();
    ~GeometryLoader();
private:
    class Impl;
    std::shared_ptr<GeometryLoader::Impl> _impl;
};

}

#endif // _MR_GEOMETRY_LOADER_H_
