
#include "ResourceBrowser.h"
#include "EditorPlusPath.h"
#include "Config.h"

#include "IconBrowser/IconBrowser.h"
#include "ColorBrowser/ColorBrowser.h"
#include "FontBrowser/FontBrowser.h"

#define LOCTEXT_NAMESPACE "EditorPlusTools"

void FResourceBrowser::OnStartup()
{
	RegisterMenu();
	StartupTools();
}

void FResourceBrowser::OnShutdown()
{
	UnregisterMenu();
	ShutdownTools();
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

	Tools.Emplace(MakeShared<FIconBrowser>());
	Tools.Emplace(MakeShared<FColorBrowser>());
	Tools.Emplace(MakeShared<FFontBrowser>());
}


#undef LOCTEXT_NAMESPACE
