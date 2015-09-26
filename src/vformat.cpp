#include "mr/vformat.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"

#include "mr/pre/glew.hpp"

#include <unordered_map>

namespace mr {

VertexDecl::Changer& VertexDecl::Changer::Pos(PosDataType const& type) {
    Push((uint32_t)type, 3, false);
    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Color(ColorDataType const& type) {
    Push((uint32_t)type, 4, true);
    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Data(uint8_t sz) {
    Push(GL_BYTE, sz, false);
    return *this;
}

void VertexDecl::Changer::End() {
    decl._size = offset;
    decl._attribs.Resize(attribi);
    for(uint8_t i = 0, n = attribs.size(); i < n; ++i) {
        Attrib a; auto const& asrc = attribs[i];
        a.bindpoint = asrc.bindpoint;
        a.datatype = asrc.gl_data_type;
        a.normalized = (asrc.normalized ? 1 : 0);
        a.offset = asrc.offset;
        a.components_num = asrc.components_num;
        decl._attribs.attribs[i] = a;
    }
}

void VertexDecl::Changer::Push(uint32_t gl_dt, uint8_t comp_num, bool norm) {
    AttribDesc a;
    a.bindpoint = attribi;
    a.offset = offset;
    a.components_num = comp_num;
    a.gl_data_type = gl_dt;
    a.normalized = norm;

    const std::unordered_map<uint32_t, uint8_t> gl_data_type_size = {
        { GL_FLOAT, sizeof(float) },
        { GL_HALF_FLOAT, sizeof(float)/2 },
        { GL_INT, sizeof(int32_t) },
        { GL_SHORT, sizeof(uint16_t) },
        { GL_UNSIGNED_BYTE, 1 },
        { GL_UNSIGNED_SHORT, sizeof(uint16_t) },
        { GL_UNSIGNED_INT, sizeof(uint32_t) }
    };

    const uint8_t sz = gl_data_type_size.at(gl_dt) * comp_num;
    a.size = sz;

    attribs.push_back(a);
    ++attribi;
    offset += sz;
}

VertexDecl::Changer::Changer(VertexDecl& d) : decl(d) {
}

VertexDecl::Changer VertexDecl::Begin() {
    return VertexDecl::Changer(*this);
}

std::future<bool> VertexDecl::Bind(uint32_t binding) {
    PromiseData<bool>* pdata = new PromiseData<bool>();
    VertexDecl* vd = this;
    auto fut = pdata->promise.get_future();

    Core::Exec([vd, binding](void* arg){
        PromiseData<bool>* parg = (PromiseData<bool>*)arg;
        PromiseData<bool>::Ptr free_guard(parg);

        parg->promise.set_value(VertexDecl::_Bind(vd, binding));
    }, pdata);

    return fut;
}

bool VertexDecl::_Bind(VertexDecl* decl, uint32_t binding) {
    for(uint8_t i = 0, n = decl->_attribs.num; i < n; ++i) {
        auto const& a = decl->_attribs.attribs[i];
        glVertexAttribFormat(a.bindpoint, a.components_num, a.datatype, a.normalized, a.offset);
        glVertexAttribBinding(a.bindpoint, binding);
    }
    return true;
}

void VertexDecl::AttribArray::Resize(uint8_t n) {
    Free();
    attribs = new Attrib[n];
    num = n;
}

void VertexDecl::AttribArray::Free() {
    if(attribs != nullptr) {
        delete [] attribs;
    }
    num = 0;
}

VertexDecl::AttribArray::~AttribArray() {
    Free();
}

}
