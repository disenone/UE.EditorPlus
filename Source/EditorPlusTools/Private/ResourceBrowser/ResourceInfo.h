
#pragma once

#include <Styling/SlateStyleRegistry.h>

class FResourceInfo
{
public:

	enum class EResourceType
	{
		Icon,
		Color,
		Font,
		WidgetStyle,
		Value,
		Sound,
		TypeNum,
		Unknown,
	};

	explicit FResourceInfo(EResourceType InType, const FString& InTypeName)
		: Type(InType)
		, TypeName(InTypeName)
	{
	}

	bool operator< (const FResourceInfo& Other) const
	{
		return Type < Other.Type;
	}

	static TSharedRef<TArray<TSharedRef<FResourceInfo>>> CollectResources();

	TSharedRef<SWidget> GetTabWidget() const;

	const EResourceType Type;
	const FString	TypeName;
};
