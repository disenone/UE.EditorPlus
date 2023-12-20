// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorPlus.h"
#include "EditorPlusLog.h"
#include "EditorPlusMenuManager.h"

DEFINE_LOG_CATEGORY(LogEditorPlus);

void FEditorPlusModule::StartupModule()
{
	if (!MenuManager.IsValid())
	{
		MenuManager = MakeShared<FEditorPlusMenuManager>();
	}
}
void FEditorPlusModule::ShutdownModule()
{
	if(MenuManager.IsValid())
	{
		MenuManager.Reset();
	}
}
	
IMPLEMENT_MODULE(FEditorPlusModule, EditorPlus)
