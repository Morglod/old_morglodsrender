#include "mr/draw.hpp"
#include "src/thread/util.hpp"
#include "mr/core.hpp"
#include "mr/vbuffer.hpp"
#include "mr/ibuffer.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

std::future<bool> Draw::Primitive(DrawMode const& dmode, VertexBufferPtr const& vb, IndexBufferPtr const& ib) {
    PromiseData<bool>* pdata = new PromiseData<bool>();

    auto vb_ = vb.get();
    IndexBuffer* ib_ = (ib == nullptr) ? nullptr : ib.get();
    uint32_t drawmode = (uint32_t)dmode;

    auto fut = pdata->promise.get_future();

    Core::Exec([vb_, ib_, drawmode](void* arg){
        PromiseData<bool>* parg = (PromiseData<bool>*)arg;
        PromiseData<bool>::Ptr free_guard(parg);

        parg->promise.set_value(Draw::_Primitive(drawmode, vb_, ib_));
    }, pdata);

    return fut;
}

bool Draw::_Primitive(uint32_t dmode, VertexBuffer* vb, IndexBuffer* ib) {
    VertexBuffer::_Bind(vb, 0, 0);
    if(ib) {
        IndexBuffer::_Bind(ib);
        glDrawElements(dmode, ib->_num, (uint32_t) ib->_dtype, ib->_mem);
    } else {
        glDrawArrays(dmode, 0, vb->_num);
    }
    return true;
}

std::future<bool> Draw::Clear(uint32_t flags) {
    PromiseData<bool>* pdata = new PromiseData<bool>();
    auto fut = pdata->promise.get_future();

    Core::Exec([flags](void* arg){
        PromiseData<bool>* parg = (PromiseData<bool>*)arg;
        PromiseData<bool>::Ptr free_guard(parg);

        parg->promise.set_value(Draw::_Clear(flags));
    }, pdata);

    return fut;
}

bool Draw::_Clear(uint32_t flags) {
    glClear(flags);
    return true;
}

std::future<bool> Draw::ClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    PromiseData<bool>* pdata = new PromiseData<bool>();
    auto fut = pdata->promise.get_future();

    Core::Exec([r, g, b, a](void* arg){
        PromiseData<bool>* parg = (PromiseData<bool>*)arg;
        PromiseData<bool>::Ptr free_guard(parg);

        parg->promise.set_value(Draw::_ClearColor(r, g, b, a));
    }, pdata);

    return fut;
}

bool Draw::_ClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    glClearColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
    return true;
}

}
