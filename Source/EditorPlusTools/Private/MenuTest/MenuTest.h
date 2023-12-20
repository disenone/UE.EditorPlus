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
	void RegisterPathSuccess(const FString& Path);
	
	TArray<TSharedPtr<FEditorPlusMenuBase>> Menus;
	TSharedPtr<SEditableTextBox> InputText;
};

DECLARE_LOG_CATEGORY_EXTERN(LogMenuTest, Display, All);
