#pragma once

#ifndef _MR_MESH_H_
#define _MR_MESH_H_

#include "Types.hpp"
#include <string>

namespace MR {

class GeometryBuffer;
class Material;

class Mesh : public Object {
public:
    inline std::string GetName() { return _name; }
    inline GeometryBuffer* GetGeometry() { return _geometry; }
    inline Material* GetMaterial() { return _material; }

    inline std::string ToString() override { return "Mesh("+_name+")"; }

    Mesh() : Object() {}
    ~Mesh();
protected:
    std::string _name;
    GeometryBuffer* _geometry;
    Material* _material;
};

}

#endif // _MR_MESH_H_
