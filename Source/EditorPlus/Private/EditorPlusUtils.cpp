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

FName FEditorPlusUtils::GenUniqueId(const FName& Name)
{
	static uint32 Id = 0;
	return FName(Name.ToString() + FString::FromInt(Id++));
}
