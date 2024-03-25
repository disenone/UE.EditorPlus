
#include "ColorInfo.h"
#include "EditorPlusUtils.h"

#include <Styling/SlateStyleRegistry.h>

// exposed Colors
using FExposedSlateColorsType = TMap<FName, FSlateColor>;
EP_EXPOSE_PRIVATE(ExposedSlateColors, FSlateStyleSet, FExposedSlateColorsType, SlateColorValues);

using FExposedLinearColorsType = TMap<FName, FLinearColor>;
EP_EXPOSE_PRIVATE(ExposedLinearColors, FSlateStyleSet, FExposedLinearColorsType, ColorValues);


TSharedRef<TArray<TSharedRef<FColorInfo>>> FColorInfo::CollectColors()
{
	static TSharedRef<TArray<TSharedRef<FColorInfo>>> StaticColors = MakeShared<TArray<TSharedRef<FColorInfo>>>();
	if(!StaticColors->IsEmpty()) return StaticColors;

	TArray<TSharedRef<FColorInfo>> Colors;
	FSlateStyleRegistry::IterateAllStyles([&Colors](const ISlateStyle& Style)
	{
		const FSlateStyleSet* StyleSet = static_cast<const FSlateStyleSet*>(&Style);
		if(!StyleSet) return true;
		const FName SetName = Style.GetStyleSetName();
		for (const auto& Elem: ExposedSlateColors(*StyleSet))
		{
			Colors.Emplace(MakeShared<FColorInfo>(EColorInfoType::SlateColor, Elem.Value, Elem.Key, SetName));
		}
		for (const auto& Elem: ExposedLinearColors(*StyleSet))
		{
			Colors.Emplace(MakeShared<FColorInfo>(EColorInfoType::SlateLinearColor, Elem.Value, Elem.Key, SetName));
		}
		return true;
	});

#define EP_LINEAR_COLOR_ENUM(Name) {FLinearColor::Name, TEXT(#Name)}
	const TArray<TTuple<FLinearColor, FName>> LinearColorEnums{
		EP_LINEAR_COLOR_ENUM(White),
		EP_LINEAR_COLOR_ENUM(Black),
		EP_LINEAR_COLOR_ENUM(Gray),
		EP_LINEAR_COLOR_ENUM(Transparent),
		EP_LINEAR_COLOR_ENUM(Red),
		EP_LINEAR_COLOR_ENUM(Green),
		EP_LINEAR_COLOR_ENUM(Blue),
		EP_LINEAR_COLOR_ENUM(Yellow),
	};
	for (const auto& Elem: LinearColorEnums)
	{
		Colors.Emplace(MakeShared<FColorInfo>(EColorInfoType::LinearColor, Elem.Get<0>(), Elem.Get<1>()));
	}
#undef EP_LINEAR_COLOR_ENUM

#define EP_COLOR_ENUM(Name) {FColor::Name, TEXT(#Name)}
	const TArray<TTuple<FColor, FName>> ColorEnums{
		EP_COLOR_ENUM(White),
		EP_COLOR_ENUM(Black),
		EP_COLOR_ENUM(Transparent),
		EP_COLOR_ENUM(Red),
		EP_COLOR_ENUM(Green),
		EP_COLOR_ENUM(Blue),
		EP_COLOR_ENUM(Cyan),
		EP_COLOR_ENUM(Magenta),
		EP_COLOR_ENUM(Orange),
		EP_COLOR_ENUM(Purple),
		EP_COLOR_ENUM(Turquoise),
		EP_COLOR_ENUM(Silver),
		EP_COLOR_ENUM(Emerald),
	};
	for (const auto& Elem: ColorEnums)
	{
		Colors.Emplace(MakeShared<FColorInfo>(EColorInfoType::Color, Elem.Get<0>(), Elem.Get<1>()));
	}
#undef EP_COLOR_ENUM

	Colors.Sort([](const TSharedRef<FColorInfo>& A, const TSharedRef<FColorInfo>& B)
	{
		return *A < *B;
	});

	*StaticColors = Colors;

	return StaticColors;
}
