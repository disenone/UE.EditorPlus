#pragma once

#include "LevelEditor.h"

class EDITORPLUS_API FEditorPlusUtils
{
public:
	static FLevelEditorModule& GetLevelEditorModule()
	{
		return FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	}

	static TArray<FString> SplitString(const FString& InString, const FString& InDelimiter = "/");

	static FName PathJoin(const FName& Left, const FName& Right, const FString& InDelimiter = "/");
};
