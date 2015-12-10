#include "mr/primitive.hpp"

namespace mr {

bool Primitive::Draw(ShaderProgramPtr const& program, uint32_t instancesNum) const {
    return Draw::Primitive(program, _drawMode, _vbuffer, _ibuffer, instancesNum);
}

PrimitivePtr Primitive::Create(DrawMode const& dmode, VertexBufferPtr const& vb, IndexBufferPtr const& ib) {
    PrimitivePtr prim = PrimitivePtr(new Primitive);
    prim->_drawMode = dmode;
    prim->_vbuffer = vb;
    prim->_ibuffer = ib;
    return prim;
}

}
