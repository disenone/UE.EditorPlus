
#pragma once

#include <Styling/SlateStyleRegistry.h>

class FWidgetStyleInfo
{
public:

	enum class EWidgetStyleType
	{
		Unknown,
		Button,
		CheckBox,
		TextBlock,
		ComboButton,
		ComboBox,
		Hyperlink,
		EditableText,
		ScrollBar,
		EditableTextBox,
		InlineEditableTextBlock,
		ProgressBar,
		ExpandableArea,
		SearchBox,
		Slider,
		VolumeControl,
		InlineTextImage,
		SpinBox,
		Splitter,
		TableView,
		TableRow,
		TableColumnHeader,
		HeaderRow,
		DockTab,
		ScrollBox,
		ScrollBorder,
		Window,
		StyleNum,
	};

	explicit FWidgetStyleInfo(const TSharedRef<FSlateWidgetStyle>& InWidgetStyle, const FName& InWidgetStyleName=NAME_None, const FName& InStyleSetName=NAME_None)
		: WidgetStyle(InWidgetStyle)
		, WidgetTypeName(InWidgetStyle->GetTypeName())
		, StyleSetName(InStyleSetName)
		, SimpleName(InWidgetStyleName.ToString())
		, FriendlyName(InStyleSetName.ToString() + TEXT(" | ") + InWidgetStyle->GetTypeName().ToString() + TEXT(" | ") + SimpleName)
		, Usage(GetUsage())
		, Type(GetType())
	{
	}

	EWidgetStyleType GetType() const;

	FString GetUsage() const
	{
		FString GetStyle;
		if (StyleSetName == FAppStyle::GetAppStyleSetName())
			GetStyle = TEXT("FAppStyle::Get().");
		else
			GetStyle = FString::Format(TEXT("FSlateStyleRegistry::FindSlateStyle(\"{0}\")->"), {StyleSetName.ToString()});

		return FString::Format(
			TEXT("{0}GetWidgetStyle<{1}>(\"{2}\");"),
			{GetStyle, WidgetTypeName.ToString(), SimpleName}
			);
	}

	static TSharedRef<TArray<TSharedRef<FWidgetStyleInfo>>> CollectWidgetStyles();

	bool operator< (const FWidgetStyleInfo& Other) const
	{
		if (WidgetTypeName != Other.WidgetTypeName)
			return WidgetTypeName.ToString() < Other.WidgetTypeName.ToString();

		return SimpleName < Other.SimpleName;
	}

	template<class Style>
	const Style* GetStyle() const
	{
		return &(FSlateStyleRegistry::FindSlateStyle(StyleSetName)->GetWidgetStyle<Style>(FName(SimpleName)));
	}

	TSharedRef<SWidget> GetSampleWidget() const;

	const TSharedRef<FSlateWidgetStyle> WidgetStyle;
	const FName		WidgetTypeName;
	const FName		StyleSetName;
	const FString	SimpleName;
	const FString	FriendlyName;
	const FString	Usage;
	const EWidgetStyleType Type;
};
