#pragma once

#ifndef _MR_MESH_H_
#define _MR_MESH_H_

#include "MeshInterfaces.hpp"

namespace MR {

class Mesh : public IMesh {
public:
    void SetGeometry(TStaticArray<IGeometry*> & geom) override { _geom = geom; OnGeometryChanged(this, geom); }
    inline TStaticArray<IGeometry*> GetGeometry() override { return _geom; }

    void SetMaterial(IMaterial* m) override { _mat = m; OnMaterialChanged(this, m); }
    inline IMaterial* GetMaterial() override { return _mat; }

    void Draw() override;

    Mesh(const TStaticArray<IGeometry*>& geom, IMaterial* mat);
    virtual ~Mesh();

    static IMesh* Create(const TStaticArray<IGeometry*>& geom, IMaterial* mat);
protected:
    TStaticArray<IGeometry*> _geom;
    IMaterial* _mat;
};

}

#endif // _MR_MESH_H_
