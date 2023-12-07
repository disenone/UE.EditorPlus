#pragma once

#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"

class FMenuTest: public IEditorPlusToolInterface
{
public:
	virtual void OnStartup() override;
	virtual void OnShutdown() override;
private:
	void BuildMenu();
	void BuildSubMenu();
	void BuildPathMenu();
	void RegisterConsoleCommand();
	FReply OnClickButton();
	
	TSharedPtr<FEditorPlusMenuBase> Menu;
	TSharedPtr<FEditorPlusMenuBase> SubMenu;
	TArray<TSharedPtr<FEditorPlusPathMenu>> PathMenus;
	TSharedPtr<SEditableTextBox> InputText;
};

DECLARE_LOG_CATEGORY_EXTERN(LogMenuTest, Display, All);
