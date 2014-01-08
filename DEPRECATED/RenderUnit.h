//!!NOT USED!!

#pragma once

#ifndef _MR_RENDER_UNIT_H_
#define _MR_RENDER_UNIT_H_

#include "MorglodsRender.h"

namespace MR{
    //---------------------------------------
    //Interface for building scene nodes tree
    //---------------------------------------
    class IRenderUnit{
    public:
        virtual void Draw() = 0;
        virtual Node* ParentNode() = 0;
        virtual void AttachTo(Node& n) = 0;
    };
}

#endif // _MR_RENDER_UNIT_H_
