#pragma once

#ifndef _MR_CORE_H_
#define _MR_CORE_H_

namespace MR {

class IContext;
class IVertexFormat;

bool Init(IContext* ctx);
void Shutdown();

}

#endif // _MR_CORE_H_
