
#pragma once

#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"
#include "EditorPlusTab.h"

class SResourceBrowserTab;

class FResourceBrowser final: public IEditorPlusToolManagerInterface
{
public:
	virtual void OnStartup() override;
	virtual void OnShutdown() override;
	virtual void AddTools() override;

private:
	void RegisterMenu();
	void UnregisterMenu();
	void RegisterTab();
	void UnregisterTab();

	TSharedPtr<FEditorPlusTab> Tab;
	TSharedPtr<FEditorPlusMenuBase> Menu;
	TSharedPtr<SResourceBrowserTab> BrowserTab;
};

DECLARE_LOG_CATEGORY_EXTERN(LogResourceBrowser, Display, All);
