
#include "IconBrowser.h"
#include "IconBrowserTab.h"
#include "EditorPlusPath.h"
#include "Config.h"

DEFINE_LOG_CATEGORY(LogIconBrowser);
#define LOCTEXT_NAMESPACE "EditorPlusTools"

void FIconBrowser::OnStartup()
{
	RegisterTab();
	RegisterMenu();
}

void FIconBrowser::OnShutdown()
{
	UnregisterMenu();
	UnregisterTab();
}

void FIconBrowser::RegisterTab()
{
	if (!Tab.IsValid())
	{
		Tab = MakeShared<FEditorPlusTab>(LOCTEXT("IconBrowser", "IconBrowser"), LOCTEXT("IconBrowserTips", "Open the IconBrowser"));
		Tab->Register(FOnSpawnTab::CreateLambda([self=SharedThis(this)](const FSpawnTabArgs& TabSpawnArgs)
		{
			TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
				.TabRole(ETabRole::NomadTab)
				.OnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda([self](TSharedRef<SDockTab>)
				{
					if (self->IconBrowserTab.IsValid())
					{
						self->IconBrowserTab->OnClose();
					}
				}))
				[
					SAssignNew(self->IconBrowserTab, SIconBrowserTab)
				];
			return SpawnedTab;
		}));
	}
}

void FIconBrowser::UnregisterTab()
{
	if (Tab.IsValid())
	{
		Tab->Unregister();
		Tab.Reset();
	}
}

void FIconBrowser::RegisterMenu()
{
	if (!Menu.IsValid())
	{
		Menu = FEditorPlusPath::RegisterPathAction(
			EP_TOOLS_PATH "/<Section>ResourceBrowser/<Command>IconBrowser",
			FExecuteAction::CreateSP(Tab.ToSharedRef(), &FEditorPlusTab::TryInvokeTab),
			EP_FNAME_HOOK_AUTO,
			LOCTEXT("IconBrowser", "IconBrowser"),
			LOCTEXT("IconBrowserTips", "Open the IconBrowser")
			);
	}
}

void FIconBrowser::UnregisterMenu()
{
	if (Menu.IsValid())
	{
		Menu->Destroy();
		Menu.Reset();
	}
}

#undef LOCTEXT_NAMESPACE
