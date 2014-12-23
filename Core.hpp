#pragma once

#ifndef _MR_CORE_H_
#define _MR_CORE_H_

namespace mr {

bool Init(class IContextManager* contextManager);
bool Init(class IContext* context);
void Shutdown();

}

#endif // _MR_CORE_H_
