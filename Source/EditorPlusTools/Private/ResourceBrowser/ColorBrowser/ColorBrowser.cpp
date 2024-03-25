
#include "ColorBrowser.h"
#include "ColorBrowserTab.h"
#include "EditorPlusPath.h"
#include "Config.h"

DEFINE_LOG_CATEGORY(LogColorBrowser);
#define LOCTEXT_NAMESPACE "EditorPlusTools"

void FColorBrowser::OnStartup()
{
	RegisterTab();
	RegisterMenu();
}

void FColorBrowser::OnShutdown()
{
	UnregisterMenu();
	UnregisterTab();
}

void FColorBrowser::RegisterTab()
{
	if (!Tab.IsValid())
	{
		Tab = MakeShared<FEditorPlusTab>(LOCTEXT("ColorBrowser", "ColorBrowser"), LOCTEXT("ColorBrowserTips", "Open the ColorBrowser"));
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
					SAssignNew(self->BrowserTab, SColorBrowserTab)
				];
			return SpawnedTab;
		}));
	}
}

void FColorBrowser::UnregisterTab()
{
	if (Tab.IsValid())
	{
		Tab->Unregister();
		Tab.Reset();
	}
}

void FColorBrowser::RegisterMenu()
{
	if (!Menu.IsValid())
	{
		Menu = FEditorPlusPath::RegisterPathAction(
			EP_TOOLS_PATH "/<Section>ResourceBrowser/<Command>ColorBrowser",
			FExecuteAction::CreateSP(Tab.ToSharedRef(), &FEditorPlusTab::TryInvokeTab),
			EP_FNAME_HOOK_AUTO,
			LOCTEXT("ColorBrowser", "ColorBrowser"),
			LOCTEXT("ColorBrowserTips", "Open the ColorBrowser")
			);
	}
}

void FColorBrowser::UnregisterMenu()
{
	if (Menu.IsValid())
	{
		Menu->Destroy();
		Menu.Reset();
	}
}

#undef LOCTEXT_NAMESPACE
