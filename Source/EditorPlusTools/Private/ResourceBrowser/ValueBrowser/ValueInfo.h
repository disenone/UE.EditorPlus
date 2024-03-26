
#pragma once

class FValueInfo
{
public:
	enum class EValueInfoType: uint8
	{
		Float,
		Vector2D,
	};

	union FValue
	{
		FValue(float InValue) { FloatValue = InValue; }
		FValue(const FVector2f& InValue) { Vector2DValue = InValue; }

		float FloatValue;
		FVector2f Vector2DValue;
	};

	FValueInfo(const EValueInfoType InType, const FValue& InValue, const FName& InValueName=NAME_None, const FName& InStyleSetName=NAME_None)
		: Type(InType)
		, Value(InValue)
		, StyleSetName(InStyleSetName)
		, SimpleName(InValueName.ToString())
		, FriendlyName(InStyleSetName.ToString() + TEXT(" | ") + SimpleName)
		, Usage(GetUsage())
		, ValueName(GetValueName())
	{
	}

	FString GetValueName() const
	{
		switch (Type)
		{
			case EValueInfoType::Float:
				return FString::Printf(TEXT("<%.2f>"), Value.FloatValue);
			case EValueInfoType::Vector2D:
				return FString::Printf(TEXT("<X=%.2f, Y=%.2f>"), Value.Vector2DValue.X, Value.Vector2DValue.Y);
		}
		return "";
	}

	FString GetUsage() const
	{
		FString GetStyle;
		if (StyleSetName == FAppStyle::GetAppStyleSetName())
			GetStyle = TEXT("FAppStyle::Get().");
		else
			GetStyle = FString::Format(TEXT("FSlateStyleRegistry::FindSlateStyle(\"{0}\")->"), {StyleSetName.ToString()});

		switch (Type)
		{
		case EValueInfoType::Float:
			return FString::Format(
				TEXT("{0}GetFloat(\"{1}\");"),
				{GetStyle, SimpleName}
				);

		case EValueInfoType::Vector2D:
			return FString::Format(
				TEXT("{0}GetVector(\"{1}\");"),
				{GetStyle, SimpleName}
				);
		}
		return "";
	}

	static TSharedRef<TArray<TSharedRef<FValueInfo>>> CollectValues();

	bool operator< (const FValueInfo& Other) const
	{
		return SimpleName < Other.SimpleName;
	}

	const EValueInfoType Type;
	const FValue	Value;
	const FName		StyleSetName;
	const FString	SimpleName;
	const FString	FriendlyName;
	const FString	Usage;
	const FString	ValueName;
};
