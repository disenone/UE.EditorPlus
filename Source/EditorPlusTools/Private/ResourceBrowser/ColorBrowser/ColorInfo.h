
#pragma once

enum class EColorInfoType: uint8
{
	SlateColor,
	SlateLinearColor,
	LinearColor,
	Color,
};

class FColorInfo
{
public:
	FColorInfo(const EColorInfoType InType, const FSlateColor& InColor, const FName& InColorName=NAME_None, const FName& InStyleSetName=NAME_None)
		: Type(InType)
		, Color(InColor)
		, StyleSetName(InStyleSetName)
		, SimpleName(GetSimpleName(InColorName))
		, FriendlyName(GetFriendlyName())
		, Usage(GetUsage())
	{
	}

	FString GetSimpleName(const FName& InColorName) const
	{
		switch (Type)
		{
		case EColorInfoType::SlateColor:
		case EColorInfoType::SlateLinearColor:
			return InColorName.ToString();
		case EColorInfoType::LinearColor:
			return TEXT("FLinearColor::") + InColorName.ToString();
		case EColorInfoType::Color:
			return TEXT("FColor::") + InColorName.ToString();
		}
		return "";
	}

	FString GetFriendlyName() const
	{
		switch (Type)
		{
		case EColorInfoType::SlateColor:
		case EColorInfoType::SlateLinearColor:
			return StyleSetName.ToString() + TEXT(" | ") + SimpleName;
		case EColorInfoType::LinearColor:
			return SimpleName;
		case EColorInfoType::Color:
			return SimpleName;
		}
		return "";
	}

	FString GetUsage() const
	{
		switch (Type)
		{
		case EColorInfoType::SlateColor:
			return FString::Format(
				TEXT("FSlateStyleRegistry::FindSlateStyle(\"{0}\")->GetSlateColor(\"{1}\");"),
				{StyleSetName.ToString(), SimpleName}
				);

		case EColorInfoType::SlateLinearColor:
			return FString::Format(
				TEXT("FSlateStyleRegistry::FindSlateStyle(\"{0}\")->GetColor(\"{1}\");"),
				{StyleSetName.ToString(), SimpleName}
				);

		case EColorInfoType::LinearColor:
			return FriendlyName;

		case EColorInfoType::Color:
			return FriendlyName;
		}
		return "";
	}

	static TSharedRef<TArray<TSharedRef<FColorInfo>>> CollectColors();

	bool operator< (const FColorInfo& Other) const
	{
		return FriendlyName < Other.FriendlyName;
	}

	const EColorInfoType Type;
	const FSlateColor Color;
	const FName		StyleSetName;
	const FString	SimpleName;
	const FString	FriendlyName;
	const FString	Usage;
};
