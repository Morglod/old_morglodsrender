#pragma once

namespace mr {

class ShaderFeatureSet {
public:
    /*
        If false, sampler2D
        If true, nv bindless texture handle
    */
    bool nvBindless = false;

    /*
        Pack all uniforms as uniform blocks and store in buffers.
    */
    bool uniformBlocks = false;
};

}
