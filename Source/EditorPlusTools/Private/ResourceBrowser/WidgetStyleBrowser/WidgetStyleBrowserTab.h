
#pragma once

#include "WidgetStyleInfo.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/STileView.h>

class SMultiLineEditableText;

class SWidgetStyleBrowserTab final : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SWidgetStyleBrowserTab)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void OnClose() {};

	using FWidgetStyleType = TSharedRef<FWidgetStyleInfo>;
	using FWidgetStyleListType = TArray<FWidgetStyleType>;

private:
	TSharedRef<SWidget> ConstructContent();
	TSharedRef<ITableRow> OnWidgetStyleTile(FWidgetStyleType InItem, const TSharedRef<STableViewBase>& OwnerTable);
	FReply OnClickWidgetStyle(const FWidgetStyleType& InItem);
	void OnHoverWidgetStyle(const FWidgetStyleType& InItem);
	void OnUnhoverWidgetStyle(const FWidgetStyleType& InItem);
	FReply OnResetWidgetStyle();
	FReply OnSelectWidgetStyle(const FString& InAction, const FWidgetStyleType& InWidgetStyle);

	void OnSearchWidgetStyle(const FText& InFilterText);
	void UpdateWidgetStyleList();

private:
	FWidgetStyleListType					WidgetStyleAllList;
	FWidgetStyleListType					WidgetStyleList;
	typedef STileView<FWidgetStyleType>	SWidgetStyleView;
	TSharedPtr<SWidgetStyleView>			WidgetStyleView;

	TSharedPtr<SSearchBox>			WidgetStyleSearchBox;
	TSharedPtr<FActiveTimerHandle>	WidgetStyleSearchTimer;
	const float						WidgetStyleSearchDelay = 0.1f;

	// usage
	TSharedPtr<FWidgetStyleInfo>			SelectedWidgetStyle;
	TSharedPtr<SMultiLineEditableText>		SelectedWidgetStyleName;
	TSharedPtr<SMultiLineEditableText>		DetailText;
	TSharedPtr<SMultiLineEditableText>		DetailWidgetStyle;
};
