
#pragma once

#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/STileView.h>
#include "IconInfo.h"

class SMultiLineEditableText;

class SIconBrowserTab final : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SIconBrowserTab)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void OnClose() {};

	using FIconType = TSharedRef<FIconInfo>;
	using FIconListType = TArray<FIconType>;

private:
	TSharedRef<SWidget> ConstructContent();
	TSharedRef<ITableRow> OnIconTile(FIconType InItem, const TSharedRef<STableViewBase>& OwnerTable);
	FReply OnClickIcon(FIconType InItem);
	void OnHoverIcon(FIconType InItem);
	void OnUnhoverIcon(FIconType InItem);

	void OnSearchIcon(const FText& InFilterText);
	void UpdateIconList();

private:
	FIconListType					IconAllList;
	FIconListType					IconList;
	typedef STileView<FIconType>	SIconView;
	TSharedPtr<SIconView>			IconView;
	TSharedPtr<FIconInfo>			SelectedIcon;

	TSharedPtr<SSearchBox>			IconSearchBox;
	TSharedPtr<FActiveTimerHandle>	IconSearchTimer;
	const float						IconSearchDelay = 0.1f;

	// usage
	TSharedPtr<SMultiLineEditableText>		DetailText;

};
