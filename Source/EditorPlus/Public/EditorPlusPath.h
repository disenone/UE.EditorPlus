﻿#pragma once

#include "EditorPlusMenu.h"

class EDITORPLUS_API FEditorPlusPath
{
public:
	static TSharedPtr<FEditorPlusMenuBase> RegisterPath(
		const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu=nullptr);
	
	static TSharedPtr<FEditorPlusMenuBase> RegisterPathAction(
		const FString& Path, const FExecuteAction& ExecuteAction, const FName& Hook=NAME_None);
	
	static bool UnregisterPath(
		const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Leaf=nullptr);
	
	static TSharedPtr<FEditorPlusMenuBase> GetNodeByPath(const FString& Path);
};