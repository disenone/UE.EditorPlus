
#include "ResourceBrowser.h"
#include "EditorPlusPath.h"
#include "Config.h"

#include "IconBrowser/IconBrowser.h"
#include "ColorBrowser/ColorBrowser.h"
#include "FontBrowser/FontBrowser.h"
#include "SoundBrowser/SoundBrowser.h"
#include "ValueBrowser/ValueBrowser.h"
#include "ResourceBrowser/ResourceBrowser.h"
#include "ResourceBrowserTab.h"

DEFINE_LOG_CATEGORY(LogResourceBrowser);
#define LOCTEXT_NAMESPACE "EditorPlusTools"


void FResourceBrowser::OnStartup()
{
	RegisterMenu();

#ifdef EP_ENABLE_RESOURCE_BROWSER_ALL_IN_ONE
	RegisterTab();
#endif

	StartupTools();
}

void FResourceBrowser::OnShutdown()
{
	ShutdownTools();
	UnregisterTab();
	UnregisterMenu();
}


void FResourceBrowser::RegisterTab()
{
	if (!Tab.IsValid())
	{
		Tab = MakeShared<FEditorPlusTab>(
			LOCTEXT("ResourceBrowser", "SlateResourceBrowser"),
			LOCTEXT("ResourceBrowserTips", "Open the SlateResourceBrowser"));
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
					SAssignNew(self->BrowserTab, SResourceBrowserTab)
				];
			return SpawnedTab;
		}));


		FEditorPlusPath::RegisterPathAction(
			EP_TOOLS_PATH "/<Section>ResourceBrowser/<Command>SlateResourceBrowser",
			FExecuteAction::CreateSP(Tab.ToSharedRef(), &FEditorPlusTab::TryInvokeTab),
			EP_FNAME_HOOK_AUTO,
			LOCTEXT("WidgetStyleBrowser", "SlateResourceBrowser"),
			LOCTEXT("WidgetStyleBrowserTips", "Open the SlateResourceBrowser")
			);
	}
}

void FResourceBrowser::UnregisterTab()
{
	if (Tab.IsValid())
	{
		Tab->Unregister();
		Tab.Reset();
	}
}


void FResourceBrowser::RegisterMenu()
{
	if (!Menu.IsValid())
	{
		Menu = FEditorPlusPath::RegisterPath(EP_TOOLS_PATH "/<Section>ResourceBrowser");
	}
}

void FResourceBrowser::UnregisterMenu()
{
	if (Menu.IsValid())
	{
		Menu->Destroy();
		Menu.Reset();
	}
}

void FResourceBrowser::AddTools()
{
	if (!Tools.IsEmpty()) return;

#ifndef EP_ENABLE_RESOURCE_BROWSER_ALL_IN_ONE

	Tools.Emplace(MakeShared<FIconBrowser>());
	Tools.Emplace(MakeShared<FColorBrowser>());
	Tools.Emplace(MakeShared<FFontBrowser>());
	Tools.Emplace(MakeShared<FSoundBrowser>());
	Tools.Emplace(MakeShared<FValueBrowser>());
	Tools.Emplace(MakeShared<FResourceBrowser>());

#endif
}


#undef LOCTEXT_NAMESPACE
