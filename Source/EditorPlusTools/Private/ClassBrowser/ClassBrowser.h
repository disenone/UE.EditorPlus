#pragma once

#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"
#include "EditorPlusTab.h"

class SClassBrowserTab;

class FClassBrowser: public IEditorPlusToolInterface
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
	TSharedPtr<FEditorPlusMenuBase> TestMenu;
	TSharedPtr<SClassBrowserTab> ClassBrowserTab;
};

DECLARE_LOG_CATEGORY_EXTERN(LogClassBrowser, Display, All);
