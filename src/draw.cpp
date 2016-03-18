#include "mr/draw.hpp"
#include "src/thread/util.hpp"
#include "mr/core.hpp"
#include "mr/vbuffer.hpp"
#include "mr/ibuffer.hpp"
#include "mr/shader/program.hpp"
#include "mr/log.hpp"

#include "src/mp.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

bool Draw::Clear(ClearFlags const& flags) {
    MP_BeginSample(Draw::Clear);
    glClear((uint32_t)flags);
    MP_EndSample();
    return true;
}

bool Draw::SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    MP_BeginSample(Draw::SetClearColor);
    glClearColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
    MP_EndSample();
    return true;
}

}
