
#include "SoundBrowser.h"
#include "SoundBrowserTab.h"
#include "EditorPlusPath.h"
#include "Config.h"

DEFINE_LOG_CATEGORY(LogSoundBrowser);
#define LOCTEXT_NAMESPACE "EditorPlusTools"

void FSoundBrowser::OnStartup()
{
	RegisterTab();
	RegisterMenu();
}

void FSoundBrowser::OnShutdown()
{
	UnregisterMenu();
	UnregisterTab();
}

void FSoundBrowser::RegisterTab()
{
	if (!Tab.IsValid())
	{
		Tab = MakeShared<FEditorPlusTab>(LOCTEXT("SoundBrowser", "SoundBrowser"), LOCTEXT("SoundBrowserTips", "Open the SoundBrowser"));
		Tab->Register(FOnSpawnTab::CreateLambda([self=SharedThis(this)](const FSpawnTabArgs& TabSpawnArgs)
		{
			TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
				.TabRole(ETabRole::NomadTab)
				.OnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda([self](TSharedRef<SDockTab>)
				{
					if (self->SoundBrowserTab.IsValid())
					{
						self->SoundBrowserTab->OnClose();
					}
				}))
				[
					SAssignNew(self->SoundBrowserTab, SSoundBrowserTab)
				];
			return SpawnedTab;
		}));
	}
}

void FSoundBrowser::UnregisterTab()
{
	if (Tab.IsValid())
	{
		Tab->Unregister();
		Tab.Reset();
	}
}

void FSoundBrowser::RegisterMenu()
{
	if (!Menu.IsValid())
	{
		Menu = FEditorPlusPath::RegisterPathAction(
			EP_TOOLS_PATH "/<Section>ResourceBrowser/<Command>SoundBrowser",
			FExecuteAction::CreateSP(Tab.ToSharedRef(), &FEditorPlusTab::TryInvokeTab),
			EP_FNAME_HOOK_AUTO,
			LOCTEXT("SoundBrowser", "SoundBrowser"),
			LOCTEXT("SoundBrowserTips", "Open the SoundBrowser")
			);
	}
}

void FSoundBrowser::UnregisterMenu()
{
	if (Menu.IsValid())
	{
		Menu->Destroy();
		Menu.Reset();
	}
}

#undef LOCTEXT_NAMESPACE
