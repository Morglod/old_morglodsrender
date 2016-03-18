#include "mr/primitive.hpp"
#include "src/thread/util.hpp"
#include "mr/core.hpp"
#include "mr/vbuffer.hpp"
#include "mr/ibuffer.hpp"
#include "mr/shader/program.hpp"
#include "mr/log.hpp"

#include "src/mp.hpp"
#include "mr/alloc.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

bool Primitive::Draw(ShaderProgramPtr const& program, uint32_t instancesNum) const {
    MP_BeginSample(Primitive::Draw);

    typedef  struct {
        uint32_t  count; // Specifies the number of indices to be rendered.
        uint32_t  primCount; // Specifies the number of instances of the specified range of indices to be rendered.
        uint32_t  first; // Specifies the starting index in the enabled arrays.
        uint32_t  baseInstance; // Specifies the base instance for use in fetching instanced vertex attributes.
    } DrawArraysIndirectCommand;

    typedef  struct {
        uint32_t  count; // Specifies the number of elements to be rendered.
        uint32_t  primCount; // Specifies the number of instances of the indexed geometry that should be drawn.
        uint32_t  firstIndex;
        uint32_t  baseVertex; // Specifies a constant that should be added to each element of indices when chosing elements from the enabled vertex arrays.
        uint32_t  baseInstance; // Specifies the base instance for use in fetching instanced vertex attributes.
    } DrawElementsIndirectCommand;

    ShaderProgram::Use(program);

    if(!_vbuffer->Bind(0, _vb_offset)) {
        MR_LOG_ERROR(Draw::Primitive, "Failed bind vertex buffer");
        MP_EndSample();
        return false;
    }

    MP_BeginSample(glDraw);

    if(_ibuffer) {
        if(!_ibuffer->Bind()) {
            MR_LOG_ERROR(Draw::Primitive, "Failed bind index buffer");
            MP_EndSample();
            MP_EndSample();
            return false;
        }

        // Offset in binded buffer or direct ptr to indecies
        size_t ioffset = (size_t)(sizeof_gl((const uint32_t)_ibuffer->_dtype) * _baseIndex);
        if(_ibuffer->_mem == nullptr) ioffset += (size_t)_ib_offset;
        else ioffset += (size_t)_ibuffer->_mem->GetPtr() + (size_t)_ib_offset;

        glDrawElementsInstancedBaseVertexBaseInstance((const uint32_t)_drawMode, _ibuffer->_num, (const uint32_t)_ibuffer->_dtype, (void*)ioffset, instancesNum, _baseVertex, _baseInstance);
    } else {
        glDrawArraysInstancedBaseInstance((const uint32_t)_drawMode, _baseVertex, _vbuffer->_num, instancesNum, _baseInstance);
    }
    MP_EndSample();

    MP_EndSample();

    return true;
}

PrimitivePtr Primitive::Create(DrawMode const& dmode, VertexBufferPtr const& vb, uint32_t vb_offset, IndexBufferPtr const& ib, uint32_t ib_offset, uint32_t baseVertex, uint32_t baseIndex, uint32_t baseInstance) {
    PrimitivePtr prim = MR_NEW_SHARED(Primitive);
    prim->_drawMode = dmode;
    prim->_vbuffer = vb;
    prim->_ibuffer = ib;
    prim->_vb_offset = vb_offset;
    prim->_ib_offset = ib_offset;
    prim->_baseVertex = baseVertex;
    prim->_baseIndex = baseIndex;
    prim->_baseInstance = baseInstance;
    return prim;
}

}
