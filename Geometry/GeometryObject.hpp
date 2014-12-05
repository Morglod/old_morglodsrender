#pragma once

#ifndef _MR_GEOMETRY_OBJECT_H_
#define _MR_GEOMETRY_OBJECT_H_

#include "GeometryInterfaces.hpp"

namespace mr {

class Geometry : public IGeometry {
public:
    IGeometryBuffer* GetGeometryBuffer() override { return _buffer; }
    void SetGeometryBuffer(IGeometryBuffer* buffer) override;

    IGeometryDrawParamsPtr GetDrawParams() override { return _draw_params; }
    void SetDrawParams(IGeometryDrawParamsPtr params) override { _draw_params = params; }

    void Draw() override;

    Geometry(IGeometryBuffer* buffer, IGeometryDrawParamsPtr params);
    virtual ~Geometry();

    static IGeometry* MakeTriangle(const float& scale = 1.0f, const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f));
    static IGeometry* MakeQuad(const glm::vec2& scale = glm::vec2(1.0f, 1.0f), const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f), const bool& texCoords = true, const glm::vec2& texCoordsScale = glm::vec2(1.0f, 1.0f));
    static IGeometry* MakeBox(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f), const bool& inside = false);
protected:
    IGeometryBuffer* _buffer;
    IGeometryDrawParamsPtr _draw_params;
};

}

#endif
