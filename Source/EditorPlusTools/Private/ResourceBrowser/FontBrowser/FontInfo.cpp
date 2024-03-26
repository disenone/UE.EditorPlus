
#include "FontInfo.h"
#include "EditorPlusUtils.h"

#include <Styling/SlateStyleRegistry.h>

#include "Styling/StarshipCoreStyle.h"

// exposed Colors
using FExposedFontsType = TMap<FName, FSlateFontInfo>;
EP_EXPOSE_PRIVATE(ExposedFonts, FSlateStyleSet, FExposedFontsType, FontInfoResources);


TSharedRef<TArray<TSharedRef<FFontInfo>>> FFontInfo::CollectFonts()
{
	static TSharedRef<TArray<TSharedRef<FFontInfo>>> StaticRet = MakeShared<TArray<TSharedRef<FFontInfo>>>();
	if(!StaticRet->IsEmpty()) return StaticRet;

	TArray<TSharedRef<FFontInfo>> Ret;
	FSlateStyleRegistry::IterateAllStyles([&Ret](const ISlateStyle& Style)
	{
		const FSlateStyleSet* StyleSet = static_cast<const FSlateStyleSet*>(&Style);
		if(!StyleSet) return true;
		const FName SetName = Style.GetStyleSetName();
		for (const auto& Elem: ExposedFonts(*StyleSet))
		{
			Ret.Emplace(MakeShared<FFontInfo>(EFontInfoType::SlateFont, Elem.Value, Elem.Key, SetName));
		}
		return true;
	});

#define EP_STYLE_FONT_ENUM(Name) {FStyleFonts::Get().Name, TEXT(#Name)}
	const TArray<TTuple<FSlateFontInfo, FName>> StyleFontEnums{
		EP_STYLE_FONT_ENUM(Normal),
		EP_STYLE_FONT_ENUM(NormalItalic),
		EP_STYLE_FONT_ENUM(NormalBold),
		EP_STYLE_FONT_ENUM(NormalBoldItalic),
		EP_STYLE_FONT_ENUM(Small),
		EP_STYLE_FONT_ENUM(SmallBold),
		EP_STYLE_FONT_ENUM(Large),
		EP_STYLE_FONT_ENUM(LargeBold),
		EP_STYLE_FONT_ENUM(HeadingMedium),
		EP_STYLE_FONT_ENUM(HeadingSmall),
		EP_STYLE_FONT_ENUM(HeadingExtraSmall),
	};
	for (const auto& Elem: StyleFontEnums)
	{
		Ret.Emplace(MakeShared<FFontInfo>(EFontInfoType::StyleFont, Elem.Get<0>(), Elem.Get<1>()));
	}
#undef EP_STYLE_FONT_ENUM

	Ret.Sort([](const TSharedRef<FFontInfo>& A, const TSharedRef<FFontInfo>& B)
	{
		return *A < *B;
	});

	*StaticRet = Ret;

	return StaticRet;
}
