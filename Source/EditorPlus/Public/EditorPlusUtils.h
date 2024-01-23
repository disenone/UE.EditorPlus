// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once

#include "LevelEditor.h"

class EDITORPLUS_API FEditorPlusUtils
{
public:
	static FString GetPathDelimiter();
	
	static FLevelEditorModule& GetLevelEditorModule()
	{
		return FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	}

	static TArray<FString> SplitString(const FString& InString, const FString& InDelimiter = GetPathDelimiter());

	static FName PathJoin(const FName& Left, const FName& Right, const FString& InDelimiter = GetPathDelimiter());

	static FText CreateFText(const FString& InTextLiteral, const FString& InNamespace, const FString& InKey);

	static FName GenUniqueId(const FName& Tag);
};
