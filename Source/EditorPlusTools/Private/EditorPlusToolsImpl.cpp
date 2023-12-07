// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorPlusToolsImpl.h"

#include "MenuTest/MenuTest.h"
#include "ClassBrowser/ClassBrowser.h"


TWeakPtr<FEditorPlusToolsImpl> FEditorPlusToolsImpl::Instance = nullptr;

void FEditorPlusToolsImpl::StartupTools()
{
	IEditorPlusToolManagerInterface::StartupTools();
	BuildMenu();
	RegisterConsoleCommand();
}

void FEditorPlusToolsImpl::ShutdownTools()
{
	DestroyMenu();
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
		
	}

}

void FEditorPlusToolsImpl::DestroyMenu()
{
	if (Menu.IsValid())
	{
		Menu->Unregister();
		Menu->ClearChildren();
	}
}

void FEditorPlusToolsImpl::BuildMenu()
{
	if (IsRunningCommandlet())
		return;

	if (!Menu.IsValid())
	{
		Menu = 
		NEW_ED_MENU(FEditorPlusMenuBar)("EditorPlusTools", "Open the EditorPlusTools Menu", "EditorPlusTools")
		->AddMenuBarExtension(TEXT("Help"), EExtensionHook::After)
		;
	}

	TArray<TSharedRef<FEditorPlusMenuBase>> ToolMenus;
	ToolMenus.Reserve(Tools.Num());
	for (const auto Tool: Tools)
	{
		ToolMenus.Emplace(NEW_ED_MENU(FEditorPlusToolMenu)(Tool));
	}
	
	Menu->AddChildren(ToolMenus);
}

void FEditorPlusToolsImpl::RegisterConsoleCommand()
{
}
