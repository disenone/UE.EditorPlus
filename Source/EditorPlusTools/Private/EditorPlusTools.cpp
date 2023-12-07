// Copyright Epic Games, Inc. All Rights Reserved.

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
