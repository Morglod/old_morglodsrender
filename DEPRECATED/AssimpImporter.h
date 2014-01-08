#ifndef _MR_ASSIMP_IMPORTER_H_
#define _MR_ASSIMP_IMPORTER_H_

#include "MorglodsRender.h"

namespace MR{
    class Mesh;

    GeometryBuffer* ImportGeometry(std::string file, std::string name);
    GeometryBuffer* ImportGeometry(std::string file, unsigned int i);
    MR::Mesh* ImportMesh(std::string file);
}

#endif
