
#include "FontBrowser.h"
#include "FontBrowserTab.h"
#include "EditorPlusPath.h"
#include "Config.h"

DEFINE_LOG_CATEGORY(LogFontBrowser);
#define LOCTEXT_NAMESPACE "EditorPlusTools"

void FFontBrowser::OnStartup()
{
	RegisterTab();
	RegisterMenu();
}

void FFontBrowser::OnShutdown()
{
	UnregisterMenu();
	UnregisterTab();
}

void FFontBrowser::RegisterTab()
{
	if (!Tab.IsValid())
	{
		Tab = MakeShared<FEditorPlusTab>(LOCTEXT("FontBrowser", "FontBrowser"), LOCTEXT("FontBrowserTips", "Open the FontBrowser"));
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
					SAssignNew(self->BrowserTab, SFontBrowserTab)
				];
			return SpawnedTab;
		}));
	}
}

void FFontBrowser::UnregisterTab()
{
	if (Tab.IsValid())
	{
		Tab->Unregister();
		Tab.Reset();
	}
}

void FFontBrowser::RegisterMenu()
{
	if (!Menu.IsValid())
	{
		Menu = FEditorPlusPath::RegisterPathAction(
			EP_TOOLS_PATH "/<Section>ResourceBrowser/<Command>FontBrowser",
			FExecuteAction::CreateSP(Tab.ToSharedRef(), &FEditorPlusTab::TryInvokeTab),
			EP_FNAME_HOOK_AUTO,
			LOCTEXT("FontBrowser", "FontBrowser"),
			LOCTEXT("FontBrowserTips", "Open the FontBrowser")
			);
	}
}

void FFontBrowser::UnregisterMenu()
{
	if (Menu.IsValid())
	{
		Menu->Destroy();
		Menu.Reset();
	}
}

#undef LOCTEXT_NAMESPACE
