#pragma once

#ifndef _MR_GEOMETRY_STREAMS_H_
#define _MR_GEOMETRY_STREAMS_H_

#include "GeometryInterfaces.hpp"
#include "../Utils/Containers.hpp"

namespace MR {

class GeometryStream : public IGeometryStream {
public:
    inline unsigned int GetGPUId() override { return _id; }
    IVertexFormat* GetBindedVertexFormat() override { return _f; }

    void BindVertexFormat(IVertexFormat* format) override;

    static GeometryStream* Get(const unsigned int& id);
    static unsigned int GetMax();

    virtual ~GeometryStream();
protected:
    GeometryStream();

    unsigned int _id;
    IVertexFormat* _f;
};

}

#endif // _MR_GEOMETRY_STREAMS_H_
