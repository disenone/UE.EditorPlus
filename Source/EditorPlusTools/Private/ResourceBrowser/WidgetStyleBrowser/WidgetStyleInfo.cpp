
#include "WidgetStyleInfo.h"
#include "EditorPlusUtils.h"

#include <Styling/SlateStyleRegistry.h>
#include <Widgets/Input/SHyperlink.h>
#include <Widgets/Input/SSearchBox.h>
#include <Widgets/Text/SInlineEditableTextBlock.h>
#include <Widgets/Notifications/SProgressBar.h>
#include <Widgets/Input/SSlider.h>
#include <Widgets/Input/SVolumeControl.h>
#include <Widgets/Input/SSpinBox.h>
#include <Widgets/Views/SHeaderRow.h>
#include <Widgets/Layout/SScrollBorder.h>


// exposed WidgetStyles
using FExposedWidgetStylesType = TMap<FName, TSharedRef<struct FSlateWidgetStyle>>;
EP_EXPOSE_PRIVATE(ExposedWidgetStyles, FSlateStyleSet, FExposedWidgetStylesType, WidgetStyleValues);


TSharedRef<TArray<TSharedRef<FWidgetStyleInfo>>> FWidgetStyleInfo::CollectWidgetStyles()
{
	static TSharedRef<TArray<TSharedRef<FWidgetStyleInfo>>> StaticRet = MakeShared<TArray<TSharedRef<FWidgetStyleInfo>>>();
	if(!StaticRet->IsEmpty()) return StaticRet;

	TArray<TSharedRef<FWidgetStyleInfo>> Ret;
	FSlateStyleRegistry::IterateAllStyles([&Ret](const ISlateStyle& Style)
	{
		const FSlateStyleSet* StyleSet = static_cast<const FSlateStyleSet*>(&Style);
		if(!StyleSet) return true;
		const FName SetName = Style.GetStyleSetName();
		for (const auto& Elem: ExposedWidgetStyles(*StyleSet))
		{
			Ret.Emplace(MakeShared<FWidgetStyleInfo>(Elem.Value, Elem.Key, SetName));
		}

		return true;
	});

	Ret.Sort([](const TSharedRef<FWidgetStyleInfo>& A, const TSharedRef<FWidgetStyleInfo>& B)
	{
		return *A < *B;
	});

	*StaticRet = Ret;

	return StaticRet;
}

FWidgetStyleInfo::EWidgetStyleType FWidgetStyleInfo::GetType() const
{
#define EP_DECLARE_STYLE_ELEM(Name) {F##Name##Style::TypeName, EWidgetStyleType::Name}
	static TMap<FName, EWidgetStyleType> TypeMap = {
		EP_DECLARE_STYLE_ELEM(Button),
		EP_DECLARE_STYLE_ELEM(CheckBox),
		EP_DECLARE_STYLE_ELEM(TextBlock),
		EP_DECLARE_STYLE_ELEM(ComboButton),
		EP_DECLARE_STYLE_ELEM(ComboBox),
		EP_DECLARE_STYLE_ELEM(Hyperlink),
		EP_DECLARE_STYLE_ELEM(EditableText),
		EP_DECLARE_STYLE_ELEM(ScrollBar),
		EP_DECLARE_STYLE_ELEM(EditableTextBox),
		EP_DECLARE_STYLE_ELEM(InlineEditableTextBlock),
		EP_DECLARE_STYLE_ELEM(ProgressBar),
		EP_DECLARE_STYLE_ELEM(ExpandableArea),
		EP_DECLARE_STYLE_ELEM(SearchBox),
		EP_DECLARE_STYLE_ELEM(Slider),
		EP_DECLARE_STYLE_ELEM(VolumeControl),
		EP_DECLARE_STYLE_ELEM(InlineTextImage),
		EP_DECLARE_STYLE_ELEM(SpinBox),
		EP_DECLARE_STYLE_ELEM(Splitter),
		EP_DECLARE_STYLE_ELEM(TableView),
		EP_DECLARE_STYLE_ELEM(TableRow),
		EP_DECLARE_STYLE_ELEM(TableColumnHeader),
		EP_DECLARE_STYLE_ELEM(HeaderRow),
		EP_DECLARE_STYLE_ELEM(DockTab),
		EP_DECLARE_STYLE_ELEM(ScrollBox),
		EP_DECLARE_STYLE_ELEM(ScrollBorder),
		EP_DECLARE_STYLE_ELEM(Window),
	};
	checkf(TypeMap.Num() == static_cast<uint8>(EWidgetStyleType::StyleNum), TEXT("some style is missing."));
#undef EP_DECLARE_STYLE_ELEM

	if (const auto Iter = TypeMap.Find(WidgetTypeName)) return *Iter;

	return EWidgetStyleType::Unknown;
}


TSharedRef<SWidget> FWidgetStyleInfo::GetSampleWidget() const
{
	TSharedPtr<SListView<TSharedRef<uint32>>> ListView;

	switch (Type)
	{
	case EWidgetStyleType::Button:
		return SNew(SButton).ButtonStyle(GetStyle<FButtonStyle>());

	case EWidgetStyleType::CheckBox:
		return SNew(SCheckBox).Style(GetStyle<FCheckBoxStyle>());

	case EWidgetStyleType::TextBlock:
		return SNew(STextBlock).TextStyle(GetStyle<FTextBlockStyle>()).Text(FText::FromString(TEXT("Text")));

	case EWidgetStyleType::ComboButton:
		return SNew(SComboButton).ComboButtonStyle(GetStyle<FComboButtonStyle>());

	case EWidgetStyleType::ComboBox:
		return SNew(SComboBox<TSharedRef<uint32>>).ComboBoxStyle(GetStyle<FComboBoxStyle>());

	case EWidgetStyleType::Hyperlink:
		return SNew(SHyperlink).Style(GetStyle<FHyperlinkStyle>()).Text(FText::FromString(TEXT("Link")));

	case EWidgetStyleType::EditableText:
		return SNew(SEditableText).Style(GetStyle<FEditableTextStyle>()).Text(FText::FromString(TEXT("EditableText")));

	case EWidgetStyleType::ScrollBar:
		return SNew(SScrollBar).Style(GetStyle<FScrollBarStyle>());

	case EWidgetStyleType::EditableTextBox:
		return SNew(SEditableTextBox).Style(GetStyle<FEditableTextBoxStyle>()).Text(FText::FromString(TEXT("EditableTextBox")));

	case EWidgetStyleType::InlineEditableTextBlock:
		return SNew(SInlineEditableTextBlock).Style(GetStyle<FInlineEditableTextBlockStyle>()).Text(FText::FromString(TEXT("InlineEditableTextBlock")));

	case EWidgetStyleType::ProgressBar:
		return SNew(SProgressBar).Style(GetStyle<FProgressBarStyle>());

	case EWidgetStyleType::ExpandableArea:
		return SNew(SExpandableArea).Style(GetStyle<FExpandableAreaStyle>());

	case EWidgetStyleType::SearchBox:
		return SNew(SSearchBox).Style(GetStyle<FSearchBoxStyle>());

	case EWidgetStyleType::Slider:
		return SNew(SSlider).Style(GetStyle<FSliderStyle>());

	case EWidgetStyleType::VolumeControl:
		return SNew(SVolumeControl).Style(GetStyle<FVolumeControlStyle>());

	case EWidgetStyleType::InlineTextImage:
		return SNew(SImage).Image(&GetStyle<FInlineTextImageStyle>()->Image);

	case EWidgetStyleType::SpinBox:
		return SNew(SSpinBox<uint32>).Style(GetStyle<FSpinBoxStyle>());

	case EWidgetStyleType::Splitter:
		return SNew(SSplitter).Style(GetStyle<FSplitterStyle>());

	case EWidgetStyleType::TableView:
		return SNew(SListView<TSharedRef<uint32>>).ListViewStyle(GetStyle<FTableViewStyle>());

	case EWidgetStyleType::HeaderRow:
		return SNew(SHeaderRow).Style(GetStyle<FHeaderRowStyle>());

	case EWidgetStyleType::ScrollBox:
		return SNew(SScrollBox).Style(GetStyle<FScrollBoxStyle>());

	case EWidgetStyleType::ScrollBorder:
		ListView = SNew(SListView<TSharedRef<uint32>>);
		return SNew(SScrollBorder, ListView.ToSharedRef()).Style(GetStyle<FScrollBorderStyle>());

	case EWidgetStyleType::TableRow:
	case EWidgetStyleType::TableColumnHeader:
	case EWidgetStyleType::DockTab:
	case EWidgetStyleType::Window:
	default:
		return SNew(STextBlock)
			.AutoWrapText(true)
			.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
			.Text(FText::FromName(WidgetTypeName));
	}

}
