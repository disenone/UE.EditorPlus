
#include "IconInfo.h"
#include "EditorPlusUtils.h"

#include <Styling/SlateStyleRegistry.h>

// exposed Icons
template struct EditorPlus::FExposedPrivate<&FSlateStyleSet::BrushResources>;
TMap<FName, FSlateBrush*>& EditorPlus::ExposePrivate(FSlateStyleSet&);
const TMap<FName, FSlateBrush*>& EditorPlus::ExposePrivate(const FSlateStyleSet&);


TSharedRef<TArray<TSharedRef<FIconInfo>>> FIconInfo::CollectIcons()
{
	static TSharedRef<TArray<TSharedRef<FIconInfo>>> StaticIcons = MakeShared<TArray<TSharedRef<FIconInfo>>>();
	if(!StaticIcons->IsEmpty()) return StaticIcons;

	TArray<TSharedRef<FIconInfo>> Icons;
	FSlateStyleRegistry::IterateAllStyles([&Icons](const ISlateStyle& Style)
	{
		UE_LOG(LogTemp, Warning, TEXT("Style: %s"), *Style.GetStyleSetName().ToString());

		const FSlateStyleSet* StyleSet = static_cast<const FSlateStyleSet*>(&Style);
		if(!StyleSet) return true;
		const FName SetName = Style.GetStyleSetName();
		for (const auto& Elem: EditorPlus::ExposePrivate(*StyleSet))
		{
			Icons.Emplace(MakeShared<FIconInfo>(FSlateIcon(SetName, Elem.Key), Elem.Value->GetImageSize()));
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
