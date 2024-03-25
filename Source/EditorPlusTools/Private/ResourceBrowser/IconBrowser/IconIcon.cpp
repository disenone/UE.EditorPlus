
#include "IconInfo.h"
#include "EditorPlusUtils.h"

#include <Styling/SlateStyleRegistry.h>

// exposed Icons
using FExposedIconsType = TMap<FName, FSlateBrush*>;
EP_EXPOSE_PRIVATE(ExposedIcons, FSlateStyleSet, FExposedIconsType, BrushResources);


TSharedRef<TArray<TSharedRef<FIconInfo>>> FIconInfo::CollectIcons()
{
	static TSharedRef<TArray<TSharedRef<FIconInfo>>> StaticIcons = MakeShared<TArray<TSharedRef<FIconInfo>>>();
	if(!StaticIcons->IsEmpty()) return StaticIcons;

	TArray<TSharedRef<FIconInfo>> Icons;
	FSlateStyleRegistry::IterateAllStyles([&Icons](const ISlateStyle& Style)
	{
		const FSlateStyleSet* StyleSet = static_cast<const FSlateStyleSet*>(&Style);
		if(!StyleSet) return true;
		const FName SetName = Style.GetStyleSetName();
		for (const auto& Elem: ExposedIcons(*StyleSet))
		{
			Icons.Emplace(MakeShared<FIconInfo>(FSlateIcon(SetName, Elem.Key), Elem.Value));
		}
		return true;
	});

	Icons.Sort([](const TSharedRef<FIconInfo>& A, const TSharedRef<FIconInfo>& B)
	{
		return *A < *B;
	});

	*StaticIcons = Icons;

	return StaticIcons;
}
