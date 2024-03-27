
#include "WidgetStyleBrowser.h"
#include "WidgetStyleBrowserTab.h"
#include "EditorPlusPath.h"
#include "Config.h"

DEFINE_LOG_CATEGORY(LogWidgetStyleBrowser);
#define LOCTEXT_NAMESPACE "EditorPlusTools"

void FWidgetStyleBrowser::OnStartup()
{
	RegisterTab();
	RegisterMenu();
}

void FWidgetStyleBrowser::OnShutdown()
{
	UnregisterMenu();
	UnregisterTab();
}

void FWidgetStyleBrowser::RegisterTab()
{
	if (!Tab.IsValid())
	{
		Tab = MakeShared<FEditorPlusTab>(LOCTEXT("WidgetStyleBrowser", "WidgetStyleBrowser"), LOCTEXT("WidgetStyleBrowserTips", "Open the WidgetStyleBrowser"));
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
					SAssignNew(self->BrowserTab, SWidgetStyleBrowserTab)
				];
			return SpawnedTab;
		}));
	}
}

void FWidgetStyleBrowser::UnregisterTab()
{
	if (Tab.IsValid())
	{
		Tab->Unregister();
		Tab.Reset();
	}
}

void FWidgetStyleBrowser::RegisterMenu()
{
	if (!Menu.IsValid())
	{
		Menu = FEditorPlusPath::RegisterPathAction(
			EP_TOOLS_PATH "/<Section>ResourceBrowser/<Command>WidgetStyleBrowser",
			FExecuteAction::CreateSP(Tab.ToSharedRef(), &FEditorPlusTab::TryInvokeTab),
			EP_FNAME_HOOK_AUTO,
			LOCTEXT("WidgetStyleBrowser", "WidgetStyleBrowser"),
			LOCTEXT("WidgetStyleBrowserTips", "Open the WidgetStyleBrowser")
			);
	}
}

void FWidgetStyleBrowser::UnregisterMenu()
{
	if (Menu.IsValid())
	{
		Menu->Destroy();
		Menu.Reset();
	}
}

#undef LOCTEXT_NAMESPACE
