#include "mr/vformat.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"
#include "mr/log.hpp"
#include "mr/gl/types.hpp"

#include "mr/pre/glew.hpp"

#include <unordered_map>

namespace mr {

void VertexDecl::BindPoint::Free() {
    if(attribs != nullptr) {
        delete [] attribs;
        attribs = nullptr;
    }
    num = 0;
}

void VertexDecl::BindPoint::Resize(uint8_t n) {
    Free();
    attribs = new Attrib[n];
    num = n;
}

VertexDecl::BindPoint::~BindPoint() {
    Free();
}

VertexDecl::Changer& VertexDecl::Changer::Pos(PosDataType const& type, uint8_t bindpoint) {
    Push(bindpoint, (uint32_t)type, 3, false, false);
    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Color(ColorDataType const& type, uint8_t bindpoint) {
    Push(bindpoint, (uint32_t)type, 4, true, false);
    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Data(uint8_t sz, uint8_t bindpoint) {
    Push(bindpoint, GL_BYTE, sz, false, true);
    return *this;
}

void VertexDecl::Changer::End() {
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
            Attrib dst_a;
            AttribDesc const& src_a = src_attribs.second.attribs[i];
            dst_a.index = src_a.index;
            dst_a.datatype = src_a.gl_data_type;
            dst_a.normalized = (src_a.normalized ? 1 : 0);
            dst_a.offset = src_a.offset;
            dst_a.components_num = src_a.components_num;
            dst_bindpoint.attribs[i] = dst_a;

            decl._size += src_a.size;
        }
    }
}

void VertexDecl::Changer::Push(uint8_t bindpoint_index, uint32_t gl_dt, uint8_t comp_num, bool norm, bool offsetOnly) {
    const uint32_t dt_size = (gl_dt != GL_HALF_FLOAT) ? sizeof_gl(gl_dt) : sizeof(float);
    const uint8_t attrib_size = dt_size * comp_num;

    BindPointDesc& bindpoint = bindpoints[bindpoint_index];

    if(offsetOnly) {
        bindpoint.offset += attrib_size;
        return;
    }

    AttribDesc a;
    a.index = bindpoint.attribi;
    a.offset = bindpoint.offset;
    a.components_num = comp_num;
    a.gl_data_type = gl_dt;
    a.normalized = norm;
    a.size = attrib_size;

    bindpoint.attribs.push_back(a);
    ++(bindpoint.attribi);
    bindpoint.offset += attrib_size;
}

VertexDecl::Changer::Changer(VertexDecl& d) : decl(d) {
}

VertexDecl::Changer VertexDecl::Begin() {
    return VertexDecl::Changer(*this);
}

std::future<bool> VertexDecl::Bind() {
    PromiseData<bool>* pdata = new PromiseData<bool>();
    VertexDecl* vd = this;
    auto fut = pdata->promise.get_future();

    Core::Exec([vd](void* arg)  -> uint8_t{
        PromiseData<bool>* parg = (PromiseData<bool>*)arg;
        PromiseData<bool>::Ptr free_guard(parg);

        parg->promise.set_value(VertexDecl::_Bind(vd));

        return 0;
    }, pdata);

    return fut;
}

bool VertexDecl::_Bind(VertexDecl* decl) {
    for(uint8_t i_bp = 0, n_bp = decl->_map.num; i_bp < n_bp; ++i_bp) { //foreach bindpoint
        BindPoint const& bindpoint = decl->_map.bindpoints[i_bp];
        for(uint8_t i_a = 0, n_a = bindpoint.num; i_a < n_a; ++i_a) { //foreach attrib
            Attrib const& attrib = bindpoint.attribs[i_a];
            glEnableVertexAttribArray(attrib.index);
            glVertexAttribFormat(attrib.index, attrib.components_num, attrib.datatype, attrib.normalized, attrib.offset);
            glVertexAttribBinding(attrib.index, bindpoint.bindpoint);
        }
    }
    return true;
}

void VertexDecl::AttribMap::Resize(uint8_t n) {
    Free();
    bindpoints = new BindPoint[n];
    num = n;
}

void VertexDecl::AttribMap::Free() {
    if(bindpoints != nullptr) {
        delete [] bindpoints;
        bindpoints = nullptr;
    }
    num = 0;
}

VertexDecl::AttribMap::~AttribMap() {
    Free();
}

VertexDeclPtr VertexDecl::Create() {
    return VertexDeclPtr(new VertexDecl());
}

}
