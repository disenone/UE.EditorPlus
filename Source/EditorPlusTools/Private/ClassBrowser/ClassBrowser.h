#pragma once

#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"
#include "EditorPlusTab.h"

class FClassBrowser: public IEditorPlusToolInterface
{
public:
	virtual void OnStartup() override;
	virtual void OnShutdown() override;
	virtual void OnBuildMenu(FMenuBuilder& MenuBuilder) override;
	virtual void OnDestroyMenu() override;
	
private:
	void BuildTestMenu();
	void DestroyTestMenu();
	
	TSharedPtr<FEditorPlusTab> Tab;
	TSharedPtr<FEditorPlusMenuBase> Menu;
	TSharedPtr<FEditorPlusMenuBase> TestMenu;
};

DECLARE_LOG_CATEGORY_EXTERN(LogClassBrowser, Display, All);
