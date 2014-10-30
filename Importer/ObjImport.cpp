#include "ObjImport.hpp"
#include "../Utils/Log.hpp"
#include "../Geometry/GeometryObject.hpp"
#include "../Geometry/GeometryManager.hpp"
#include "../Geometry/GeometryFormats.hpp"

namespace MR {

MR::IGeometry* ObjImporter::MakeFromFile(std::string const& file) {
/*
    std::vector<float> vertexData;
    for(size_t i = 0; i < meshPtr->positions.size(); i += 3) {
        vertexData.push_back(meshPtr->positions[i]);
        vertexData.push_back(meshPtr->positions[i+1]);
        vertexData.push_back(meshPtr->positions[i+2]);

        /*if(meshPtr->normals.size() != 0) {
            vertexData.push_back(meshPtr->normals[i]);
            vertexData.push_back(meshPtr->normals[i+1]);
            vertexData.push_back(meshPtr->normals[i+2]);
        }*/
/*
        if(meshPtr->texcoords.size() != 0) {
            vertexData.push_back(meshPtr->texcoords[i]);
            vertexData.push_back(meshPtr->texcoords[i+1]);
        }*/
    //}

    return nullptr; /*MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, &(meshPtr->positions[0]), meshPtr->positions.size() / 3,
                                                            iformat, &meshPtr->indices[0], meshPtr->indices.size(),
                                                            IGPUBuffer::Static, IGeometryBuffer::DrawModes::Triangles);*/
}

}
