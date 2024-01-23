// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#include "EditorPlusUtils.h"


FString FEditorPlusUtils::GetPathDelimiter()
{
	static const FString Delimiter = "/";
	return Delimiter;
}

TArray<FString> FEditorPlusUtils::SplitString(const FString& InString, const FString& InDelimiter)
{
	FString Left = InString;
	FString Right;
	TArray<FString> Ret;
	
	while(!Left.IsEmpty())
	{
		const bool bSucceed = Left.Split(InDelimiter, &Left, &Right);
		if (!bSucceed)
		{
			Ret.Push(Left);
			break;
		}
		if (!Left.IsEmpty())
		{
			Ret.Push(Left);
			Left.Empty();
		}
		if (!Right.IsEmpty())
		{
			Left = Right;
		}
	}

	return Ret;
}

FName FEditorPlusUtils::PathJoin(const FName& Left, const FName& Right, const FString& InDelimiter)
{
	return FName(Left.ToString() + InDelimiter + Right.ToString());
}



FText FEditorPlusUtils::CreateFText(const FString& InTextLiteral, const FString& InNamespace, const FString& InKey)
{
	return FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText(
			ToCStr(InTextLiteral), *InNamespace, ToCStr(InKey));
}


FName FEditorPlusUtils::GenUniqueId(const FName& Tag)
{
	static TMap<FName, uint32> IdMap;

	if (!IdMap.Contains(Tag))
	{
		IdMap.Add(Tag, 0);
	}

	return FName(Tag.ToString() + FString::FromInt(IdMap[Tag]++));
}
