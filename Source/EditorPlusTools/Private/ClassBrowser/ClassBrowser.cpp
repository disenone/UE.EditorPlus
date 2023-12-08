#include "ClassBrowser.h"
#include "ClassBrowserTab.h"

DEFINE_LOG_CATEGORY(LogClassBrowser);

void FClassBrowser::OnStartup()
{
	if (!Tab.IsValid())
	{
		Tab = MakeShared<FEditorPlusTab>("ClassBrowser");
	}
	// Tab->Register<SClassBrowserTab>();
	// BuildTestMenu();
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

void FClassBrowser::OnShutdown()
{
	if (Tab.IsValid())
	{
		Tab->Unregister();
	}
	DestroyTestMenu();
}

void FClassBrowser::OnBuildMenu(FMenuBuilder& MenuBuilder)
{
	if (!Menu.IsValid())
	{
		Menu = NEW_ED_MENU(FEditorPlusMenu)(
			"ClassBrowser",
			"Browser UClass, UEnum, UScriptStruct in UE",
			FExecuteAction::CreateSP(Tab.ToSharedRef(), &FEditorPlusTab::TryInvokeTab));
	}
	Menu->Register(MenuBuilder, NAME_None);
}

void FClassBrowser::OnDestroyMenu()
{
}

void FClassBrowser::DestroyTestMenu()
{
}

void FClassBrowser::BuildTestMenu()
{
	if (!TestMenu.IsValid())
	{
		TestMenu = 
		NEW_ED_MENU(FEditorPlusMenu)(
			"ClassBrowser",
			"Open the ClassBrowser Tab",
			FExecuteAction::CreateSP(Tab.ToSharedRef(), &FEditorPlusTab::TryInvokeTab))
		->AddMenuExtension(TEXT("Miscellaneous"), EExtensionHook::Before, "ClassBrowser")
		;
	}
}
