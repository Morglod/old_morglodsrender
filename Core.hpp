#pragma once

namespace mr {

bool Init(class IContextManager* contextManager);
bool Init(class IContext* context);
void Shutdown();

}
