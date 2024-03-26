
#include "ValueBrowser.h"
#include "ValueBrowserTab.h"
#include "EditorPlusPath.h"
#include "Config.h"

DEFINE_LOG_CATEGORY(LogValueBrowser);
#define LOCTEXT_NAMESPACE "EditorPlusTools"

void FValueBrowser::OnStartup()
{
	RegisterTab();
	RegisterMenu();
}

void FValueBrowser::OnShutdown()
{
	UnregisterMenu();
	UnregisterTab();
}

void FValueBrowser::RegisterTab()
{
	if (!Tab.IsValid())
	{
		Tab = MakeShared<FEditorPlusTab>(LOCTEXT("ValueBrowser", "ValueBrowser"), LOCTEXT("ValueBrowserTips", "Open the ValueBrowser"));
		Tab->Register(FOnSpawnTab::CreateLambda([self=SharedThis(this)](const FSpawnTabArgs& TabSpawnArgs)
		{
			TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
				.TabRole(ETabRole::NomadTab)
				.OnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda([self](TSharedRef<SDockTab>)
				{
					if (self->BrowserTab.IsValid())
					{
						self->BrowserTab->OnClose();
					}
				}))
				[
					SAssignNew(self->BrowserTab, SValueBrowserTab)
				];
			return SpawnedTab;
		}));
	}
}

void FValueBrowser::UnregisterTab()
{
	if (Tab.IsValid())
	{
		Tab->Unregister();
		Tab.Reset();
	}
}

void FValueBrowser::RegisterMenu()
{
	if (!Menu.IsValid())
	{
		Menu = FEditorPlusPath::RegisterPathAction(
			EP_TOOLS_PATH "/<Section>ResourceBrowser/<Command>ValueBrowser",
			FExecuteAction::CreateSP(Tab.ToSharedRef(), &FEditorPlusTab::TryInvokeTab),
			EP_FNAME_HOOK_AUTO,
			LOCTEXT("ValueBrowser", "ValueBrowser"),
			LOCTEXT("ValueBrowserTips", "Open the ValueBrowser")
			);
	}
}

void FValueBrowser::UnregisterMenu()
{
	if (Menu.IsValid())
	{
		Menu->Destroy();
		Menu.Reset();
	}
}

#undef LOCTEXT_NAMESPACE
