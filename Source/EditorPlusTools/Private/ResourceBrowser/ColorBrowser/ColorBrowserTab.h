
#pragma once

#include "ColorInfo.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/STileView.h>

class SMultiLineEditableText;

class SColorBrowserTab final : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SColorBrowserTab)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void OnClose() {};

	using FColorType = TSharedRef<FColorInfo>;
	using FColorListType = TArray<FColorType>;

private:
	TSharedRef<SWidget> ConstructContent();
	TSharedRef<ITableRow> OnColorTile(FColorType InItem, const TSharedRef<STableViewBase>& OwnerTable);
	FReply OnClickColor(const FColorType& InItem);
	void OnHoverColor(const FColorType& InItem);
	void OnUnhoverColor(const FColorType& InItem);
	FReply OnResetColor();
	FReply OnSelectColor(const FString& InAction, const FColorType& InColor);

	void OnSearchColor(const FText& InFilterText);
	void UpdateColorList();

private:
	FColorListType					ColorAllList;
	FColorListType					ColorList;
	typedef STileView<FColorType>	SColorView;
	TSharedPtr<SColorView>			ColorView;

	TSharedPtr<SSearchBox>			ColorSearchBox;
	TSharedPtr<FActiveTimerHandle>	ColorSearchTimer;
	const float						ColorSearchDelay = 0.1f;

	// usage
	TSharedPtr<FColorInfo>			SelectedColor;
	TSharedPtr<SMultiLineEditableText>		SelectedColorName;
	TSharedPtr<SMultiLineEditableText>		DetailText;
	TSharedPtr<SImage>				DetailColor;
};
