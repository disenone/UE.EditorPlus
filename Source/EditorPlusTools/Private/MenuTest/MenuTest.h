// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

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
	void BuildTestMenu();
	void BuildToolBar();
	FReply OnClickButton() const;
	TSharedPtr<FEditorPlusMenuBase> RegisterPath(const FString& Path, const bool ShouldSuccess=true, const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty());
	
	TArray<TSharedPtr<FEditorPlusMenuBase>> Menus;
	TSharedPtr<SEditableTextBox> InputText;
	TSharedPtr<FUICommandList> CommandList;
};

DECLARE_LOG_CATEGORY_EXTERN(LogMenuTest, Display, All);
