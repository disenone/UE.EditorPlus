
#pragma once

class FIconInfo
{
public:
	FIconInfo(const FSlateIcon& InIcon, const FSlateBrush* InBursh)
		: Icon(InIcon)
		, ResourceName(InBursh->GetResourceName())
		, Size(InBursh->GetImageSize())
		, SimpleName(InIcon.GetStyleName().ToString())
		, FriendlyName(InIcon.GetStyleSetName().ToString() + TEXT(" | ") + InIcon.GetStyleName().ToString())
		, Usage(GetUsage())
	{
	}

	FString GetUsage() const
	{
		if (Icon.GetStyleSetName() == FAppStyle::GetAppStyleSetName())
		{
			return FString::Format(
					TEXT("const FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), \"{0}\");"),
					{ Icon.GetStyleName().ToString() }
				);
		}
		else
		{
			return FString::Format(
				TEXT("const FSlateIcon Icon(\"{0}\", \"{1}\");"),
				{ Icon.GetStyleSetName().ToString(), Icon.GetStyleName().ToString() }
			);
		}
	}

	static TSharedRef<TArray<TSharedRef<FIconInfo>>> CollectIcons();

	bool operator< (const FIconInfo& Other) const
	{
		return FriendlyName < Other.FriendlyName;
	}

	const FSlateIcon Icon;
	const FName ResourceName;
	const FVector2f Size;
	const FString SimpleName;
	const FString FriendlyName;
	const FString Usage;
};
