#include "mr/draw.hpp"
#include "src/thread/util.hpp"
#include "mr/core.hpp"
#include "mr/vbuffer.hpp"
#include "mr/ibuffer.hpp"
#include "mr/shader/program.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

bool Draw::Primitive(ShaderProgramPtr const& program, DrawMode const& dmode, VertexBufferPtr const& vbuf, IndexBufferPtr const& ibuf) {
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

    glUseProgram(program->_id);
    vbuf->Bind(0, 0);
    const uint32_t baseInstance = 0, instancesNum = 1, baseVertex = 0, baseIndex = 0;
    if(ibuf) {
        IndexBuffer::_Bind(ibuf.get());
        void* ioffset = 0;
        if(ibuf->_mem == nullptr) ioffset = (void*)(size_t)(sizeof_gl((const uint32_t)ibuf->_dtype) * baseIndex);
        else ioffset = ibuf->_mem->GetPtr();
        glDrawElementsInstancedBaseVertexBaseInstance((const uint32_t)dmode, ibuf->_num, (const uint32_t)ibuf->_dtype, ioffset, instancesNum, baseVertex, baseInstance);
    } else {
        glDrawArraysInstancedBaseInstance((const uint32_t)dmode, baseVertex, vbuf->_num, instancesNum, baseInstance);
    }
    return true;
}

bool Draw::Clear(uint32_t flags) {
    glClear(flags);
    return true;
}

bool Draw::ClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    glClearColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
    return true;
}

}
