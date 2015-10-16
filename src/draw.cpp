#include "mr/draw.hpp"
#include "src/thread/util.hpp"
#include "mr/core.hpp"
#include "mr/vbuffer.hpp"
#include "mr/ibuffer.hpp"
#include "mr/shader/program.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

std::future<bool> Draw::Primitive(ShaderProgramPtr const& program, DrawMode const& dmode, VertexBufferPtr const& vb, IndexBufferPtr const& ib) {
    if(Core::IsWorkerThread()) {
        std::promise<bool> promise;
        auto futur = promise.get_future();
        promise.set_value(Draw::_Primitive(program.get(),
                                           (const uint32_t)dmode,
                                           vb.get(),
                                           (ib == nullptr) ? nullptr : ib.get()
                                           ));
        return futur;
    }
    else {
        PromiseData<bool>* pdata = new PromiseData<bool>();

        auto vb_ = vb.get();
        IndexBuffer* ib_ = (ib == nullptr) ? nullptr : ib.get();
        const uint32_t drawmode = (const uint32_t)dmode;
        ShaderProgram* prog = program.get();

        auto fut = pdata->promise.get_future();

        Core::Exec([prog, vb_, ib_, drawmode](void* arg) -> uint8_t{
            PromiseData<bool>* parg = (PromiseData<bool>*)arg;
            PromiseData<bool>::Ptr free_guard(parg);

            parg->promise.set_value(Draw::_Primitive(prog, drawmode, vb_, ib_));
            return 0;
        }, pdata);

        return fut;
    }
}

bool Draw::_Primitive(ShaderProgram* program, uint32_t dmode, VertexBuffer* vb, IndexBuffer* ib) {
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
    VertexBuffer::_Bind(vb, 0, 0);
    const uint32_t baseInstance = 0, instancesNum = 1, baseVertex = 0, baseIndex = 0;
    if(ib) {
        IndexBuffer::_Bind(ib);
        glDrawElementsInstancedBaseVertexBaseInstance(dmode, ib->_num, (const uint32_t)ib->_dtype, (void*)(size_t)(sizeof_gl((const uint32_t)ib->_dtype) * baseIndex), instancesNum, baseVertex, baseInstance);
    } else {
        glDrawArraysInstancedBaseInstance(dmode, baseVertex, vb->_num, instancesNum, baseInstance);
    }
    return true;
}

std::future<bool> Draw::Clear(uint32_t flags) {
    if(Core::IsWorkerThread()) {
        std::promise<bool> promise;
        auto futur = promise.get_future();
        promise.set_value(Draw::_Clear(flags));
        return futur;
    }
    else {
        PromiseData<bool>* pdata = new PromiseData<bool>();
        auto fut = pdata->promise.get_future();

        Core::Exec([flags](void* arg) -> uint8_t{
            PromiseData<bool>* parg = (PromiseData<bool>*)arg;
            PromiseData<bool>::Ptr free_guard(parg);

            parg->promise.set_value(Draw::_Clear(flags));
            return 0;
        }, pdata);

        return fut;
    }
}

bool Draw::_Clear(uint32_t flags) {
    glClear(flags);
    return true;
}

std::future<bool> Draw::ClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if(Core::IsWorkerThread()) {
        std::promise<bool> promise;
        auto futur = promise.get_future();
        promise.set_value(Draw::_ClearColor(r, g, b, a));
        return futur;
    }
    else {
        PromiseData<bool>* pdata = new PromiseData<bool>();
        auto fut = pdata->promise.get_future();

        Core::Exec([r, g, b, a](void* arg) -> uint8_t{
            PromiseData<bool>* parg = (PromiseData<bool>*)arg;
            PromiseData<bool>::Ptr free_guard(parg);

            parg->promise.set_value(Draw::_ClearColor(r, g, b, a));
            return 0;
        }, pdata);

        return fut;
    }
}

bool Draw::_ClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    glClearColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
    return true;
}

}
