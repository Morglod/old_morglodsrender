#include "GeometryDrawParams.hpp"
#include "../MachineInfo.hpp"

namespace MR {

void GeometryDrawParams::SetIndexStart(const unsigned int& i) {
    _istart = i;
    if(MR::MachineInfo::IsIndirectDrawSupported()) _MakeDrawCmd();
}

void GeometryDrawParams::SetVertexStart(const unsigned int& v) {
    _vstart = v;
    if(MR::MachineInfo::IsIndirectDrawSupported()) _MakeDrawCmd();
}

void GeometryDrawParams::SetIndexCount(const unsigned int& i) {
    _icount = i;
    if(MR::MachineInfo::IsIndirectDrawSupported()) _MakeDrawCmd();
}

void GeometryDrawParams::SetVertexCount(const unsigned int& i) {
    _vcount = i;
    if(MR::MachineInfo::IsIndirectDrawSupported()) _MakeDrawCmd();
}

void GeometryDrawParams::SetUseIndexBuffer(const bool& state) {
    _index_buffer = state;
    if(MR::MachineInfo::IsIndirectDrawSupported()) _MakeDrawCmd();
}

bool GeometryDrawParams::GetUseIndexBuffer() {
    return _index_buffer;
}

void GeometryDrawParams::_MakeDrawCmd() {
    if(_index_buffer) {
        if(drawCmd) {
            delete ((MR::IGeometryDrawParams::DrawElementsIndirectCmd*)drawCmd);
        }
        drawCmd = new MR::IGeometryDrawParams::DrawElementsIndirectCmd {_icount, 1, _istart, _vstart, 0};
    } else {
        if(drawCmd) {
            delete ((MR::IGeometryDrawParams::DrawArraysIndirectCmd*)drawCmd);
        }
        drawCmd = new MR::IGeometryDrawParams::DrawArraysIndirectCmd {_vcount, 1, _vstart, 0};
    }
}

GeometryDrawParams::GeometryDrawParams(const bool& indexBuffer, const unsigned int& istart, const unsigned int& vstart, const unsigned int& icount, const unsigned int& vcount)
 : drawCmd(0), _istart(istart), _vstart(vstart), _icount(icount), _vcount(vcount), _index_buffer(indexBuffer) {
     if(MR::MachineInfo::IsIndirectDrawSupported()) _MakeDrawCmd();
}

GeometryDrawParams::~GeometryDrawParams() {
    if(drawCmd) {
        if(_index_buffer) {
            delete ((MR::IGeometryDrawParams::DrawElementsIndirectCmd*)drawCmd);
        } else {
            delete ((MR::IGeometryDrawParams::DrawArraysIndirectCmd*)drawCmd);
        }
        drawCmd = 0;
    }
}

IGeometryDrawParamsPtr GeometryDrawParams::DrawArrays(const unsigned int& vstart, const unsigned int& vcount) {
    return IGeometryDrawParamsPtr(new GeometryDrawParams(false, 0, vstart, 0, vcount));
}

IGeometryDrawParamsPtr GeometryDrawParams::DrawElements(const unsigned int& istart, const unsigned int& icount, const unsigned int& vstart) {
    return IGeometryDrawParamsPtr(new GeometryDrawParams(true, istart, vstart, icount, 0));
}

}
