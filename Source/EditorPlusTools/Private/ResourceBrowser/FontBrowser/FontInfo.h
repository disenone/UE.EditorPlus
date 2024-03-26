
#pragma once

class FFontInfo
{
public:
	enum class EFontInfoType: uint8
	{
		SlateFont,
		StyleFont,
	};

	FFontInfo(const EFontInfoType InType, const FSlateFontInfo& InFont, const FName& InFontName=NAME_None, const FName& InStyleSetName=NAME_None)
		: Type(InType)
		, Font(InFont)
		, StyleSetName(InStyleSetName)
		, SimpleName(GetSimpleName(InFontName))
		, FriendlyName(GetFriendlyName())
		, Usage(GetUsage())
		, TypefaceFontName(!InFont.TypefaceFontName.IsNone() ? InFont.TypefaceFontName : FName(SimpleName))
	{
	}

	FString GetSimpleName(const FName& InFontName) const
	{
		switch (Type)
		{
		case EFontInfoType::SlateFont:
			return InFontName.ToString();
		case EFontInfoType::StyleFont:
			return TEXT("FStyleFonts::Get().") + InFontName.ToString();
		}
		return "";
	}

	FString GetFriendlyName() const
	{
		switch (Type)
		{
		case EFontInfoType::SlateFont:
			return StyleSetName.ToString() + TEXT(" | ") + SimpleName;
		case EFontInfoType::StyleFont:
			return SimpleName;
		}
		return "";
	}

	FString GetUsage() const
	{
		FString GetStyle;
		switch (Type)
		{
		case EFontInfoType::SlateFont:
			if (StyleSetName == FAppStyle::GetAppStyleSetName())
				GetStyle = TEXT("FAppStyle::Get().");
			else
				GetStyle = FString::Format(TEXT("FSlateStyleRegistry::FindSlateStyle(\"{0}\")->"), {StyleSetName.ToString()});

			return FString::Format(
				TEXT("{0}GetFontStyle(\"{1}\");"),
				{GetStyle, SimpleName}
				);

		case EFontInfoType::StyleFont:
			return SimpleName;

		}
		return "";
	}

	static TSharedRef<TArray<TSharedRef<FFontInfo>>> CollectFonts();

	bool operator< (const FFontInfo& Other) const
	{
		return SimpleName < Other.SimpleName;
	}

	const EFontInfoType Type;
	const FSlateFontInfo Font;
	const FName		StyleSetName;
	const FString	SimpleName;
	const FString	FriendlyName;
	const FString	Usage;
	const FName		TypefaceFontName;
};
