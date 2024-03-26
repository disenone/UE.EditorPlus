
#pragma once

class FSoundInfo
{
public:
	FSoundInfo(const FSlateSound& InSound, const FName& InSoundName=NAME_None, const FName& InStyleSetName=NAME_None)
		: Sound(InSound)
		, StyleSetName(InStyleSetName)
		, SimpleName(InSoundName.ToString())
		, FriendlyName(InStyleSetName.ToString() + TEXT(" | ") + InSoundName.ToString())
		, Usage(GetUsage())
	{
	}

	FString GetUsage() const
	{
		FString GetStyle;
		if (StyleSetName == FAppStyle::GetAppStyleSetName())
			GetStyle = TEXT("FAppStyle::Get().");
		else
			GetStyle = FString::Format(TEXT("FSlateStyleRegistry::FindSlateStyle(\"{0}\")->"), {StyleSetName.ToString()});

		return FString::Format(
			TEXT("FSlateApplication::Get().PlaySound({0}GetSound(\"{1}\"));"),
			{GetStyle, SimpleName}
			);
	}

	static TSharedRef<TArray<TSharedRef<FSoundInfo>>> CollectSounds();

	bool operator< (const FSoundInfo& Other) const
	{
		return FriendlyName < Other.FriendlyName;
	}

	const FSlateSound Sound;
	const FName StyleSetName;
	const FString SimpleName;
	const FString FriendlyName;
	const FString Usage;
};
