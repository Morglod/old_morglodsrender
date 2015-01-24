#include "GeometryDrawParams.hpp"
#include "../MachineInfo.hpp"

namespace mr {

void GeometryDrawParams::SetIndexStart(const unsigned int& i) {
    _istart = i;
    if(mr::gl::IsIndirectDrawSupported()) _MakeDrawCmd();
}

void GeometryDrawParams::SetVertexStart(const unsigned int& v) {
    _vstart = v;
    if(mr::gl::IsIndirectDrawSupported()) _MakeDrawCmd();
}

void GeometryDrawParams::SetIndexCount(const unsigned int& i) {
    _icount = i;
    if(mr::gl::IsIndirectDrawSupported()) _MakeDrawCmd();
}

void GeometryDrawParams::SetVertexCount(const unsigned int& i) {
    _vcount = i;
    if(mr::gl::IsIndirectDrawSupported()) _MakeDrawCmd();
}

void GeometryDrawParams::SetInstancesNum(unsigned int const& num) {
    _instNum = num;
    if(mr::gl::IsIndirectDrawSupported()) _MakeDrawCmd();
}

void GeometryDrawParams::SetUsingIndexBuffer(const bool& state) {
    _index_buffer = state;
    if(mr::gl::IsIndirectDrawSupported()) _MakeDrawCmd();
}

bool GeometryDrawParams::GetUsingIndexBuffer() const {
    return _index_buffer;
}

void GeometryDrawParams::_MakeDrawCmd() {
    if(_index_buffer) {
        if(drawCmd) {
            delete ((mr::IGeometryDrawParams::DrawElementsIndirectCmd*)drawCmd);
        }
        drawCmd = new mr::IGeometryDrawParams::DrawElementsIndirectCmd {_icount, _instNum, _istart, _vstart, 0};
    } else {
        if(drawCmd) {
            delete ((mr::IGeometryDrawParams::DrawArraysIndirectCmd*)drawCmd);
        }
        drawCmd = new mr::IGeometryDrawParams::DrawArraysIndirectCmd {_vcount, _instNum, _vstart, 0};
    }
}

GeometryDrawParams::GeometryDrawParams(const bool& indexBuffer, const unsigned int& istart, const unsigned int& vstart, const unsigned int& icount, const unsigned int& vcount, unsigned int const& instancesNum)
 : drawCmd(0), _istart(istart), _vstart(vstart), _icount(icount), _vcount(vcount), _index_buffer(indexBuffer), _instNum(instancesNum) {
     if(mr::gl::IsIndirectDrawSupported()) _MakeDrawCmd();
}

GeometryDrawParams::~GeometryDrawParams() {
    if(drawCmd) {
        if(_index_buffer) {
            delete ((mr::IGeometryDrawParams::DrawElementsIndirectCmd*)drawCmd);
        } else {
            delete ((mr::IGeometryDrawParams::DrawArraysIndirectCmd*)drawCmd);
        }
        drawCmd = 0;
    }
}

IGeometryDrawParamsPtr GeometryDrawParams::DrawArrays(const unsigned int& vstart, const unsigned int& vcount, unsigned int const& instancesNum) {
    return IGeometryDrawParamsPtr(new GeometryDrawParams(false, 0, vstart, 0, vcount, instancesNum));
}

IGeometryDrawParamsPtr GeometryDrawParams::DrawElements(const unsigned int& istart, const unsigned int& icount, const unsigned int& vstart, unsigned int const& instancesNum) {
    return IGeometryDrawParamsPtr(new GeometryDrawParams(true, istart, vstart, icount, 0, instancesNum));
}

}
