// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorPlusToolsImpl.h"

#include "MenuTest/MenuTest.h"
#include "ClassBrowser/ClassBrowser.h"
#include "MenuCollection/MenuCollection.h"


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
