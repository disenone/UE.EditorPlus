#pragma once

#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"

class FMenuTest: public IEditorPlusToolInterface
{
public:
	virtual void OnStartup() override;
	virtual void OnShutdown() override;
private:
	void BuildPathMenu();
	void BuildCustomMenu();
	void BuildMixMenu();
	FReply OnClickButton() const;
	void RegisterPathSuccess(const FString& Path);
	
	TArray<TSharedPtr<FEditorPlusMenuBase>> Menus;
	TSharedPtr<SEditableTextBox> InputText;
};

DECLARE_LOG_CATEGORY_EXTERN(LogMenuTest, Display, All);
