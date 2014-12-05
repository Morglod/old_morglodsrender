#pragma once

#ifndef _MR_CORE_H_
#define _MR_CORE_H_

namespace mr {

class IContext;

bool Init(IContext* ctx);
void Shutdown();

}

#endif // _MR_CORE_H_
