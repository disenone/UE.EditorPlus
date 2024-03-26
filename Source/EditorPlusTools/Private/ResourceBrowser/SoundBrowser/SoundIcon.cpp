
#include "SoundInfo.h"
#include "EditorPlusUtils.h"

#include <Styling/SlateStyleRegistry.h>

// exposed Sounds
using FExposedSoundsType =TMap<FName, FSlateSound>;
EP_EXPOSE_PRIVATE(ExposedSounds, FSlateStyleSet, FExposedSoundsType, Sounds);


TSharedRef<TArray<TSharedRef<FSoundInfo>>> FSoundInfo::CollectSounds()
{
	static TSharedRef<TArray<TSharedRef<FSoundInfo>>> StaticSounds = MakeShared<TArray<TSharedRef<FSoundInfo>>>();
	if(!StaticSounds->IsEmpty()) return StaticSounds;

	TArray<TSharedRef<FSoundInfo>> Sounds;
	FSlateStyleRegistry::IterateAllStyles([&Sounds](const ISlateStyle& Style)
	{
		const FSlateStyleSet* StyleSet = static_cast<const FSlateStyleSet*>(&Style);
		if(!StyleSet) return true;
		const FName SetName = Style.GetStyleSetName();
		for (const auto& Elem: ExposedSounds(*StyleSet))
		{
			Sounds.Emplace(MakeShared<FSoundInfo>(Elem.Value, Elem.Key, SetName));
		}
		return true;
	});

	Sounds.Sort([](const TSharedRef<FSoundInfo>& A, const TSharedRef<FSoundInfo>& B)
	{
		return *A < *B;
	});

	*StaticSounds = Sounds;

	return StaticSounds;
}
