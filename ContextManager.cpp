#include "ContextManager.hpp"

mr::IContextManager* __MR_Current_ContextManager = nullptr;

mr::IContextManager* mr::IContextManager::GetCurrent() { return __MR_Current_ContextManager; }
void mr::IContextManager::SetCurrent(IContextManager* ctx) { __MR_Current_ContextManager = ctx; }

mr::IContext::~IContext() {}
mr::IContextManager::~IContextManager() {}
