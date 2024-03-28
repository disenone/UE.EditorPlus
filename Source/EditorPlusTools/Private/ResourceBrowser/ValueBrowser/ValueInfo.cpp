
#include "ValueInfo.h"
#include "EditorPlusUtils.h"

#include <Styling/SlateStyleRegistry.h>

#include "Styling/StarshipCoreStyle.h"

// exposed Floats
using FExposedFloatsType = TMap<FName, float>;
EP_EXPOSE_PRIVATE(ExposedFloats, FSlateStyleSet, FExposedFloatsType, FloatValues);

// exposed FVector2Ds
using FExposedFVector2DsType = TMap<FName, FExposedFVector2DType>;
EP_EXPOSE_PRIVATE(ExposedFVector2Ds, FSlateStyleSet, FExposedFVector2DsType, Vector2DValues);


TSharedRef<TArray<TSharedRef<FValueInfo>>> FValueInfo::CollectValues()
{
	static TSharedRef<TArray<TSharedRef<FValueInfo>>> StaticRet = MakeShared<TArray<TSharedRef<FValueInfo>>>();
	if(!StaticRet->IsEmpty()) return StaticRet;

	TArray<TSharedRef<FValueInfo>> Ret;
	FSlateStyleRegistry::IterateAllStyles([&Ret](const ISlateStyle& Style)
	{
		const FSlateStyleSet* StyleSet = static_cast<const FSlateStyleSet*>(&Style);
		if(!StyleSet) return true;
		const FName SetName = Style.GetStyleSetName();
		for (const auto& Elem: ExposedFloats(*StyleSet))
		{
			Ret.Emplace(MakeShared<FValueInfo>(EValueInfoType::Float, Elem.Value, Elem.Key, SetName));
		}
		for (const auto& Elem: ExposedFVector2Ds(*StyleSet))
		{
			Ret.Emplace(MakeShared<FValueInfo>(EValueInfoType::Vector2D, Elem.Value, Elem.Key, SetName));
		}

		return true;
	});

	Ret.Sort([](const TSharedRef<FValueInfo>& A, const TSharedRef<FValueInfo>& B)
	{
		return *A < *B;
	});

	*StaticRet = Ret;

	return StaticRet;
}
