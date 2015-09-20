#include "GeometryObject.hpp"
#include "../MachineInfo.hpp"
#include "GeometryFormats.hpp"
#include "GeometryManager.hpp"
#include "../Utils/Debug.hpp"

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

#include <mu/Macro.hpp>

#include <GL/glew.h>

unsigned int _MR_DRAW_MODE_FLAG_TO_GL_[4] {
    GL_POINTS,
    GL_LINES,
    GL_TRIANGLES,
    GL_QUADS
};

namespace mr {

void Geometry::SetGeometryBuffer(IGeometryBufferPtr const& buffer) {
    _buffer = buffer;
}

void Geometry::Draw() const {
    if(!_buffer || !_draw_params) return;

    _buffer->Bind(_draw_params->GetUsingIndexBuffer());

    if(_buffer->GetIndexBuffer() != nullptr && _draw_params->GetUsingIndexBuffer()){
        const mr::IndexFormat* iformat = _buffer->GetIndexBuffer()->GetFormat().get();
        if(mr::gl::IsIndirectDrawSupported()) {
            glDrawElementsIndirect( _MR_DRAW_MODE_FLAG_TO_GL_[(int)_buffer->GetDrawMode()], iformat->dataType->dataTypeGL, _draw_params->GetPtr());
        }
        else {
            if(GL_ARB_base_instance) {
                glDrawElementsInstancedBaseVertexBaseInstance(_MR_DRAW_MODE_FLAG_TO_GL_[(int)_buffer->GetDrawMode()],
                                                        _draw_params->GetIndexCount(),
                                                        iformat->dataType->dataTypeGL,
                                                        (void*)(size_t)(iformat->dataType->size * _draw_params->GetIndexStart()),
                                                        _draw_params->GetInstancesNum(),
                                                        _draw_params->GetVertexStart(),
                                                        _draw_params->GetFirstInstance());
            } else {
                MU_ONE_TIME(mr::Log::LogString("GL_ARB_base_instance not supported. May be problems with IGeometryDrawParams::FirstInstance.", MR_LOG_LEVEL_ERROR));
                glDrawElementsInstancedBaseVertex(_MR_DRAW_MODE_FLAG_TO_GL_[(int)_buffer->GetDrawMode()],
                                        _draw_params->GetIndexCount(),
                                        iformat->dataType->dataTypeGL,
                                        (void*)(size_t)(iformat->dataType->size * _draw_params->GetIndexStart()),
                                        _draw_params->GetInstancesNum(),
                                        _draw_params->GetVertexStart());
            }
        }
    }
    else {
        if(mr::gl::IsIndirectDrawSupported()) glDrawArraysIndirect( _MR_DRAW_MODE_FLAG_TO_GL_[(int)_buffer->GetDrawMode()], _draw_params->GetPtr());
        else {
            if(GL_ARB_base_instance) {
                glDrawArraysInstancedBaseInstance(_MR_DRAW_MODE_FLAG_TO_GL_[(int)_buffer->GetDrawMode()], _draw_params->GetVertexStart(), _draw_params->GetVertexCount(), _draw_params->GetInstancesNum(), _draw_params->GetFirstInstance());
            }
            else {
                MU_ONE_TIME(mr::Log::LogString("GL_ARB_base_instance not supported. May be problems with IGeometryDrawParams::FirstInstance.", MR_LOG_LEVEL_ERROR));
                glDrawArraysInstanced(_MR_DRAW_MODE_FLAG_TO_GL_[(int)_buffer->GetDrawMode()], _draw_params->GetVertexStart(), _draw_params->GetVertexCount(), _draw_params->GetInstancesNum());
            }
        }
    }
}

Geometry::Geometry(IGeometryBufferPtr const& buffer, IGeometryDrawParamsPtr const& params)
 : _buffer(buffer), _draw_params(params) {
}

Geometry::~Geometry(){
}

}
