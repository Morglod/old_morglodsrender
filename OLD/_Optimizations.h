#pragma once

#ifndef _MR_OPTIMIZATIONS_H_
#define _MR_OPTIMIZATIONS_H_

#include "Node.h"
#include "GeometryBuffer.h"

namespace MR{
    //------------------------------------------------------------------------------------------
    //Optimizes drawing same geom buffers or meshes on children nodes (not children of children)
    class Optimization_SameGeometryBuffer : public virtual Node{
    public:
        MR::GeometryBuffer* main_gb = NULL;
        MR::Material* material = NULL; //if null, not used

        void Draw();
    };

    //------------------------------------------------------------------------------------------
    //Optimizes drawing same geom buffers or meshes on children nodes (not children of children)
    /*class Optimization_SameMesh : public virtual Node{
    public:
        MR::Mesh* main_m = NULL;
        MR::Material* material = NULL; //if null, not used

        void Draw();
    };*/
}

#endif
