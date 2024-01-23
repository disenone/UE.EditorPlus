// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#include "EditorPlusTools.h"
#include "EditorPlusToolsLog.h"
#include "EditorPlusToolsImpl.h"

DEFINE_LOG_CATEGORY(LogEditorPlusTools);

void FEditorPlusToolsModule::StartupModule()
{
	Impl = FEditorPlusToolsImpl::Get();
	Impl->StartupTools();

}
void FEditorPlusToolsModule::ShutdownModule()
{
	Impl->ShutdownTools();
}

IMPLEMENT_MODULE(FEditorPlusToolsModule, EditorPlusTools)
