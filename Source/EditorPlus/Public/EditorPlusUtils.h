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

	static FName GenUniqueId(const FName& Name);
};
