
#pragma once

#include "FontInfo.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/STileView.h>

class SMultiLineEditableText;

class SFontBrowserTab final : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SFontBrowserTab)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void OnClose() {};

	using FFontType = TSharedRef<FFontInfo>;
	using FFontListType = TArray<FFontType>;

private:
	TSharedRef<SWidget> ConstructContent();
	TSharedRef<ITableRow> OnFontTile(FFontType InItem, const TSharedRef<STableViewBase>& OwnerTable);
	FReply OnClickFont(const FFontType& InItem);
	void OnHoverFont(const FFontType& InItem);
	void OnUnhoverFont(const FFontType& InItem);
	FReply OnResetFont();
	FReply OnSelectFont(const FString& InAction, const FFontType& InFont);
	void OnDemonstrateChange(const FText& InText);

	void OnSearchFont(const FText& InFilterText);
	void UpdateFontList();

private:
	FFontListType					FontAllList;
	FFontListType					FontList;
	typedef STileView<FFontType>	SFontView;
	TSharedPtr<SFontView>			FontView;

	TSharedPtr<SSearchBox>			FontSearchBox;
	TSharedPtr<FActiveTimerHandle>	FontSearchTimer;
	const float						FontSearchDelay = 0.1f;

	// usage
	TSharedPtr<FFontInfo>			SelectedFont;
	TSharedPtr<SMultiLineEditableText>		SelectedFontName;
	TSharedPtr<SMultiLineEditableText>		DetailText;
	TSharedPtr<SMultiLineEditableText>		DetailFont;
	TSharedPtr<SMultiLineEditableText>		DemonstrateText;
	FText									DemonstrateContent;
};
