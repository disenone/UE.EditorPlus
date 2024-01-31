// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#include "ClassBrowser.h"
#include "ClassBrowserTab.h"
#include "EditorPlusPath.h"
#include "Config.h"

DEFINE_LOG_CATEGORY(LogClassBrowser);
#define LOCTEXT_NAMESPACE "EditorPlusTools"

void FClassBrowser::OnStartup()
{
	RegisterTab();
	RegisterMenu();
}

void FClassBrowser::OnShutdown()
{
	UnregisterMenu();
	UnregisterTab();
}

void FClassBrowser::RegisterTab()
{
	if (!Tab.IsValid())
	{
		Tab = MakeShared<FEditorPlusTab>(LOCTEXT("ClassBrowser", "ClassBrowser"), LOCTEXT("ClassBrowserTip", "Open the ClassBrowser"));
		Tab->Register(FOnSpawnTab::CreateLambda([self=SharedThis(this)](const FSpawnTabArgs& TabSpawnArgs)
		{
			TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
				.TabRole(ETabRole::NomadTab)
				.OnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda([self](TSharedRef<SDockTab>)
				{
					if (self->ClassBrowserTab.IsValid())
					{
						self->ClassBrowserTab->OnClose();
					}
				}))
				[
					SAssignNew(self->ClassBrowserTab, SClassBrowserTab)
				];
			return SpawnedTab;
		}));
	}
}

void FClassBrowser::UnregisterTab()
{
	if (Tab.IsValid())
	{
		Tab->Unregister();
		Tab.Reset();
	}
}

void FClassBrowser::RegisterMenu()
{
	if (!Menu.IsValid())
	{
		Menu = FEditorPlusPath::RegisterPathAction(
			EP_TOOLS_PATH "/<Command>ClassBrowser",
			FExecuteAction::CreateSP(Tab.ToSharedRef(), &FEditorPlusTab::TryInvokeTab),
			EP_FNAME_HOOK_AUTO,
			LOCTEXT("ClassBrowser", "ClassBrowser"),
			LOCTEXT("ClassBrowserTip", "Open the ClassBrowser")
			);
	}
}

void FClassBrowser::UnregisterMenu()
{
	if (Menu.IsValid())
	{
		Menu->Destroy();
		Menu.Reset();
	}
}

#undef LOCTEXT_NAMESPACE
