// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#include "EditorPlusToolsImpl.h"

#include "EditorPlusPath.h"
#include "MenuTest/MenuTest.h"
#include "ClassBrowser/ClassBrowser.h"
#include "MenuCollection/MenuCollection.h"

#define LOCTEXT_NAMESPACE "EditorPlusTools"

TWeakPtr<FEditorPlusToolsImpl> FEditorPlusToolsImpl::Instance = nullptr;

void FEditorPlusToolsImpl::StartupTools()
{
	IEditorPlusToolManagerInterface::StartupTools();
	RegisterConsoleCommand();
}

void FEditorPlusToolsImpl::ShutdownTools()
{
	IEditorPlusToolManagerInterface::ShutdownTools();
}

void FEditorPlusToolsImpl::AddTools()
{
	if (!Root.IsValid())
	{
		Root = FEditorPlusPath::RegisterPath(
			"/<MenuBar>EditorPlusTools",
			LOCTEXT("EditorPlusTools", "EditorPlusTools"),
			LOCTEXT("EditorPlusToolsTips", "Useful tools for editor"));
	}

	if (!Tools.Num())
	{
		
#ifdef EP_ENABLE_MENU_TEST
		Tools.Emplace(MakeShared<FMenuTest>());
#endif
	
#ifdef EP_ENABLE_CLASS_BROWSER
		Tools.Emplace(MakeShared<FClassBrowser>());
#endif

#ifdef EP_ENABLE_MENU_COLLECTION
		Tools.Emplace(MakeShared<FMenuCollection>());
#endif
	}
}

void FEditorPlusToolsImpl::RegisterConsoleCommand()
{
}

#undef LOCTEXT_NAMESPACE
