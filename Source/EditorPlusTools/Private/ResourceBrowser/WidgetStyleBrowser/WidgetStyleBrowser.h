
#pragma once


#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"
#include "EditorPlusTab.h"

class SWidgetStyleBrowserTab;

class FWidgetStyleBrowser final: public IEditorPlusToolInterface
{
public:
	virtual void OnStartup() override;
	virtual void OnShutdown() override;

private:
	void RegisterTab();
	void UnregisterTab();
	void RegisterMenu();
	void UnregisterMenu();

	TSharedPtr<FEditorPlusTab> Tab;
	TSharedPtr<FEditorPlusMenuBase> Menu;
	TSharedPtr<SWidgetStyleBrowserTab> BrowserTab;
};

DECLARE_LOG_CATEGORY_EXTERN(LogWidgetStyleBrowser, Display, All);
