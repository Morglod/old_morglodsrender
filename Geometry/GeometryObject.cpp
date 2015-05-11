#include "GeometryObject.hpp"
#include "../MachineInfo.hpp"
#include "GeometryFormats.hpp"
#include "GeometryManager.hpp"
#include "../Utils/Debug.hpp"

#include <Macro.hpp>

#define GLEW_STATIC
#include <GL/glew.h>

unsigned int _MR_DRAW_MODE_FLAG_TO_GL_[4] {
    GL_POINTS,
    GL_LINES,
    GL_TRIANGLES,
    GL_QUADS
};

namespace mr {

void Geometry::SetGeometryBuffer(IGeometryBuffer* buffer) {
    _buffer = buffer;
}

void Geometry::Draw() const {
    if(!_buffer || !_draw_params) return;

    _buffer->Bind(_draw_params->GetUsingIndexBuffer());

    if(_buffer->GetIndexBuffer() != nullptr && _draw_params->GetUsingIndexBuffer()){
        const mr::IndexFormat* iformat = _buffer->GetIndexFormat().get();
        if(mr::gl::IsIndirectDrawSupported()) {
            glDrawElementsIndirect( _MR_DRAW_MODE_FLAG_TO_GL_[_buffer->GetDrawMode()], iformat->dataType->dataTypeGL, _draw_params->GetIndirectPtr());
        }
        else {
            /*glDrawElementsBaseVertex(  _MR_DRAW_MODE_FLAG_TO_GL_[_buffer->GetDrawMode()],
                                        _draw_params->GetIndexCount(),
                                        iformat->GetDataType()->GetDataType(),
                                        (void*)(size_t)(iformat->GetSize() * _draw_params->GetIndexStart()),
                                        _draw_params->GetVertexStart());*/
            if(GL_ARB_base_instance) {
                glDrawElementsInstancedBaseVertexBaseInstance(_MR_DRAW_MODE_FLAG_TO_GL_[_buffer->GetDrawMode()],
                                                        _draw_params->GetIndexCount(),
                                                        iformat->dataType->dataTypeGL,
                                                        (void*)(size_t)(iformat->dataType->size * _draw_params->GetIndexStart()),
                                                        _draw_params->GetInstancesNum(),
                                                        _draw_params->GetVertexStart(),
                                                        _draw_params->GetFirstInstance());
            } else {
                MU_ONE_TIME(mr::Log::LogString("GL_ARB_base_instance not supported. May be problems with IGeometryDrawParams::FirstInstance.", MR_LOG_LEVEL_ERROR));
                glDrawElementsInstancedBaseVertex(_MR_DRAW_MODE_FLAG_TO_GL_[_buffer->GetDrawMode()],
                                        _draw_params->GetIndexCount(),
                                        iformat->dataType->dataTypeGL,
                                        (void*)(size_t)(iformat->dataType->size * _draw_params->GetIndexStart()),
                                        _draw_params->GetInstancesNum(),
                                        _draw_params->GetVertexStart());
            }
        }
    }
    else {
        if(mr::gl::IsIndirectDrawSupported()) glDrawArraysIndirect( _MR_DRAW_MODE_FLAG_TO_GL_[_buffer->GetDrawMode()], _draw_params->GetIndirectPtr());
        else {
            if(GL_ARB_base_instance) {
                glDrawArraysInstancedBaseInstance(_MR_DRAW_MODE_FLAG_TO_GL_[_buffer->GetDrawMode()], _draw_params->GetVertexStart(), _draw_params->GetVertexCount(), _draw_params->GetInstancesNum(), _draw_params->GetFirstInstance());
            }
            else {
                MU_ONE_TIME(mr::Log::LogString("GL_ARB_base_instance not supported. May be problems with IGeometryDrawParams::FirstInstance.", MR_LOG_LEVEL_ERROR));
                glDrawArraysInstanced(_MR_DRAW_MODE_FLAG_TO_GL_[_buffer->GetDrawMode()], _draw_params->GetVertexStart(), _draw_params->GetVertexCount(), _draw_params->GetInstancesNum());
            }
        }
    }
}

Geometry::Geometry(IGeometryBuffer* buffer, IGeometryDrawParamsPtr params)
 : _buffer(buffer), _draw_params(params) {
}

Geometry::~Geometry(){
}

//DEFAULT GEOMETRY SHAPES
IGeometry* Geometry::MakeTriangle(const float& scale, const glm::vec3& offset) {
    /*mr::VertexFormatCustomFixed vformat;
    vformat.SetAttributesNum(1);
    vformat.SetVertexAttribute(&(mr::VertexAttributePos3F::GetInstance()), 0);
    vformat.Complete();

    float vdata[] {
        offset.x,                   offset.y + 1.0f * scale, offset.z,
        offset.x + 1.0f * scale,    offset.y - 1.0f * scale, offset.z,
        offset.x - 1.0f * scale,    offset.y - 1.0f * scale, offset.z
    };

    return mr::GeometryManager::GetInstance()->PlaceGeometry(vformat.Cache(), &vdata[0], 3,
                                                                                nullptr, 0, 0,
                                                                                IGPUBuffer::Static, IGeometryBuffer::DrawMode::Triangles);*/
}

IGeometry* Geometry::MakeQuad(const glm::vec2& scale, const glm::vec3& offset, const bool& texCoords, const glm::vec2& texCoordsScale) {
    /*mr::VertexFormatCustomFixed vformat;
    vformat.SetAttributesNum((texCoords) ? (2) : (1));
    vformat.SetVertexAttribute(&(mr::VertexAttributePos3F::GetInstance()), 0);
    if(texCoords) vformat.SetVertexAttribute(mr::VertexAttributeCustom(2, &(mr::VertexDataTypeFloat::GetInstance()), MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION).Cache(), 1);
    vformat.Complete();

    mr::IndexFormatCustom iformat(VertexDataTypeCustom(GL_UNSIGNED_BYTE, 1).Cache());

    float vdata_noTexCoords[] {
        offset.x,                   offset.y + scale.y, offset.z,
        offset.x + scale.x,         offset.y + scale.y, offset.z,
        offset.x + scale.x,         offset.y,           offset.z,
        offset.x,                   offset.y,           offset.z
    };

    float vdata_texCoords[] {
        offset.x,                   offset.y + scale.y,         offset.z,  //p
        0.0f,                       0.0f,
        offset.x + scale.x,         offset.y + scale.y,         offset.z, //p
        texCoordsScale.x,           0.0f,
        offset.x + scale.x,         offset.y,                   offset.z, //p
        texCoordsScale.x,           texCoordsScale.y,
        offset.x,                   offset.y,                   offset.z, //p
        0.0f,                       texCoordsScale.y
    };

    float* vdata = (texCoords) ? (&vdata_texCoords[0]) : (&vdata_noTexCoords[0]);

    unsigned char idata[] {
        0, 1, 2,
        2, 3, 0
    };

    //IGeometryBuffer* buffer = MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, iformat, &vdata[0], sizeof(float) * (texCoords ? 20 : 12), &idata[0], sizeof(unsigned char) * 6, GL_STATIC_DRAW, GL_READ_ONLY, GL_TRIANGLES, nullptr, nullptr);
    //Geometry* geom = new MR::Geometry(buffer, GeometryDrawParams::DrawElements(0, 6));
    //return geom;

    return mr::GeometryManager::GetInstance()->PlaceGeometry(vformat.Cache(), &vdata[0], 4,
                                                            iformat.Cache(), &idata[0], 6,
                                                            IGPUBuffer::Static, IGeometryBuffer::DrawMode::Triangles);*/
}

IGeometry* Geometry::MakeBox(const glm::vec3& scale, const glm::vec3& offset, const bool& inside) {
    /*mr::VertexFormatCustomFixed vformat;
    vformat.SetAttributesNum(1);
    vformat.SetVertexAttribute(&(mr::VertexAttributePos3F::GetInstance()), 0);
    vformat.Complete();

    mr::IndexFormatCustom iformat(VertexDataTypeCustom(GL_UNSIGNED_BYTE, 1).Cache());

    float vdata[] {
        //front face
        offset.x,                   offset.y,                 offset.z,
        offset.x + 1.0f * scale.x,  offset.y,                 offset.z,
        offset.x + 1.0f * scale.x,  offset.y + 1.0f * scale.y,offset.z,
        offset.x,                   offset.y + 1.0f * scale.y,offset.z,

        //back face
        offset.x,                   offset.y,                 offset.z + 1.0f * scale.z,
        offset.x + 1.0f * scale.x,  offset.y,                 offset.z + 1.0f * scale.z,
        offset.x + 1.0f * scale.x,  offset.y + 1.0f * scale.y,offset.z + 1.0f * scale.z,
        offset.x,                   offset.y + 1.0f * scale.y,offset.z + 1.0f * scale.z
    };

    unsigned char idata_normal[] {
        2, 1, 0,
        0, 3, 2,

        6, 5, 1,
        1, 2, 6,

        3, 0, 4,
        4, 7, 3,

        6, 2, 3,
        3, 7, 6,

        7, 4, 5,
        5, 6, 7,

        1, 5, 4,
        4, 0, 1
    };

    unsigned char idata_reversed[] {
        0, 1, 2,
        2, 3, 0,

        1, 5, 6,
        6, 2, 1,

        4, 0, 3,
        3, 7, 4,

        3, 2, 6,
        6, 7, 3,

        5, 4, 7,
        7, 6, 5,

        4, 5, 1,
        1, 0, 4
    };

    unsigned char* idata = (inside) ? &idata_reversed[0] : &idata_normal[0];

    //IGeometryBuffer* buffer = MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, iformat, &vdata[0], sizeof(float) * 24, &idata[0], sizeof(unsigned char) * 36, GL_STATIC_DRAW, GL_READ_ONLY, GL_TRIANGLES, nullptr, nullptr);
    //Geometry* geom = new MR::Geometry(buffer, GeometryDrawParams::DrawElements(0, 36));
    return mr::GeometryManager::GetInstance()->PlaceGeometry(vformat.Cache(), &vdata[0], 8,
                                                            iformat.Cache(), &idata[0], 36,
                                                            IGPUBuffer::Static, IGeometryBuffer::DrawMode::Triangles);*/
}

}
