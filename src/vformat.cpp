#include "mr/vformat.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"
#include "mr/log.hpp"
#include "mr/gl/types.hpp"
#include "src/mp.hpp"
#include "mr/pre/glew.hpp"

#include <unordered_map>

namespace mr {

void VertexDecl::BindPoint::Free() {
    MP_BeginSample(VertexDecl::BindPoint::Free);

    if(attribs != nullptr) {
        delete [] attribs;
        attribs = nullptr;
    }
    num = 0;

    MP_EndSample();
}

void VertexDecl::BindPoint::Resize(uint8_t n) {
    MP_BeginSample(VertexDecl::BindPoint::Resize);

    Free();
    attribs = new Attrib[n];
    num = n;

    MP_EndSample();
}

VertexDecl::BindPoint::~BindPoint() {
    Free();
}

VertexDecl::Changer& VertexDecl::Changer::Pos(PosDataType const& type, uint8_t bindpoint) {
    MP_BeginSample(VertexDecl::Changer::Pos);

    Push(bindpoint, (uint32_t)type, 3, false, false);

    MP_EndSample();

    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Color(ColorDataType const& type, uint8_t bindpoint) {
    MP_BeginSample(VertexDecl::Changer::Color);

    Push(bindpoint, (uint32_t)type, 4, true, false);

    MP_EndSample();

    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Data(uint8_t sz, uint8_t bindpoint) {
    MP_BeginSample(VertexDecl::Changer::Data);

    Push(bindpoint, GL_BYTE, sz, false, true);

    MP_EndSample();

    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Custom(DataType const& type, uint8_t components_num, uint8_t bindpoint, bool normalized) {
    MP_BeginSample(VertexDecl::Changer::Custom);

    Push(bindpoint, (uint32_t)type, components_num, normalized, false);

    MP_EndSample();
    return *this;
}

void VertexDecl::Changer::End() {
    MP_BeginSample(VertexDecl::Changer::End);

    // Write changes
    // src - VertexDecl::Changer declaration
    // dst - VertexDecl declaration

    decl._size = 0;
    decl._map.Resize(bindpoints.size());
    uint8_t ibp = 0;
    for(std::pair<uint8_t, BindPointDesc> const& src_attribs : bindpoints) {
        BindPoint& dst_bindpoint = decl._map.bindpoints[ibp++];
        dst_bindpoint.Resize(src_attribs.second.attribs.size());
        dst_bindpoint.bindpoint = src_attribs.first;
        dst_bindpoint.stride = src_attribs.second.offset;
        for(uint8_t i = 0, n = src_attribs.second.attribs.size(); i < n; ++i) {
            dst_bindpoint.attribs[i] = src_attribs.second.attribs[i];
            decl._size += dst_bindpoint.attribs[i].size;
        }
    }

    MP_EndSample();
}

void VertexDecl::Changer::Push(uint8_t bindpoint_index, uint32_t gl_dt, uint8_t comp_num, bool norm, bool offsetOnly) {
    MP_BeginSample(VertexDecl::Changer::Push);

    const uint32_t dt_size = (gl_dt != GL_HALF_FLOAT) ? sizeof_gl(gl_dt) : sizeof(float);
    const uint8_t attrib_size = dt_size * comp_num;

    BindPointDesc& bindpoint = bindpoints[bindpoint_index];

    if(offsetOnly) {
        bindpoint.offset += attrib_size;
        MP_EndSample();
        return;
    }

    Attrib attrib;
    attrib.index = bindpoint.attribi;
    attrib.offset = bindpoint.offset;
    attrib.components_num = comp_num;
    attrib.datatype = gl_dt;
    attrib.normalized = norm;
    attrib.size = attrib_size;

    bindpoint.attribs.push_back(attrib);
    ++(bindpoint.attribi);
    bindpoint.offset += attrib_size;

    MP_EndSample();
}

VertexDecl::Changer::Changer(VertexDecl& d) : decl(d) {
}

VertexDecl::Changer VertexDecl::Begin() {
    return VertexDecl::Changer(*this);
}

bool VertexDecl::Bind() {
    MP_BeginSample(VertexDecl::Bind);

    for(uint8_t i_bp = 0, n_bp = _map.num; i_bp < n_bp; ++i_bp) { //foreach bindpoint
        BindPoint const& bindpoint = _map.bindpoints[i_bp];
        for(uint8_t i_a = 0, n_a = bindpoint.num; i_a < n_a; ++i_a) { //foreach attrib
            Attrib const& attrib = bindpoint.attribs[i_a];
            glEnableVertexAttribArray(attrib.index);
            if(GLEW_NV_vertex_buffer_unified_memory) {
                glVertexAttribFormatNV(attrib.index, attrib.components_num, attrib.datatype, attrib.normalized, bindpoint.stride);
            }
            else {
                glVertexAttribFormat(attrib.index, attrib.components_num, attrib.datatype, attrib.normalized, attrib.offset);
                glVertexAttribBinding(attrib.index, bindpoint.bindpoint);
            }
        }
    }

    MP_EndSample();

    return true;
}

void VertexDecl::AttribMap::Resize(uint8_t n) {
    MP_BeginSample(VertexDecl::AttribMap::Resize);

    Free();
    bindpoints = new BindPoint[n];
    num = n;

    MP_EndSample();
}

void VertexDecl::AttribMap::Free() {
    MP_BeginSample(VertexDecl::AttribMap::Free);

    if(bindpoints != nullptr) {
        delete [] bindpoints;
        bindpoints = nullptr;
    }
    num = 0;

    MP_EndSample();
}

VertexDecl::AttribMap::~AttribMap() {
    Free();
}

VertexDeclPtr VertexDecl::Create() {
    return VertexDeclPtr(new VertexDecl());
}

}
