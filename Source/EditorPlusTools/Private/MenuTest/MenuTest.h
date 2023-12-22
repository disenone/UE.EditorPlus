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
	void BuildExtendMenu();
	FReply OnClickButton() const;
	void RegisterPath(const FString& Path, const bool ShouldSuccess=true);
	
	TArray<TSharedPtr<FEditorPlusMenuBase>> Menus;
	TSharedPtr<SEditableTextBox> InputText;
};

DECLARE_LOG_CATEGORY_EXTERN(LogMenuTest, Display, All);
