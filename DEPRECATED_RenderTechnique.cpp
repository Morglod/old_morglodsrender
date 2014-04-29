#include "RenderTechnique.hpp"

#include <GL/glew.h>

namespace MR {

void RenderTechnique::Use(IRenderSystem* rc){
    if(_depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);

    if(_cull_face) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
}

}
