
#pragma once


#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"
#include "EditorPlusTab.h"

class SColorBrowserTab;

class FColorBrowser final: public IEditorPlusToolInterface
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
	TSharedPtr<SColorBrowserTab> BrowserTab;
};

DECLARE_LOG_CATEGORY_EXTERN(LogColorBrowser, Display, All);
