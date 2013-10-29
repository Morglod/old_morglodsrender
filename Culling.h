#pragma once

#ifndef _MR_CULLING_H_
#define _MR_CULLING_H_

#include "MorglodsRender.h"

namespace MR{
    class Node;

    void DrawCulled(MR::Node* node);
}

#endif
