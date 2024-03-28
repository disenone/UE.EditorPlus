
#pragma once

#include "ResourceInfo.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/STileView.h>

class SMultiLineEditableText;

class SResourceBrowserTab final : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SResourceBrowserTab)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void OnClose() {};

	using FResourceType = TSharedRef<FResourceInfo>;
	using FResourceListType = TArray<FResourceType>;

private:
	TSharedRef<SWidget> ConstructContent();
	TSharedRef<ITableRow> OnGenerateTabButton(FResourceType InItem, const TSharedRef<STableViewBase>& OwnerTable);
	FReply OnClickTabButton(const FResourceType& InItem);

private:
	FResourceListType					ResourceAllList;
	typedef SListView<FResourceType>	SResourceTabView;
	TSharedPtr<SResourceTabView>		ResourceTabView;

	TSharedPtr<SBox>					ResourceTabBox;
	TSharedPtr<FResourceInfo>			SelectedTab;
	TMap<FResourceInfo::EResourceType, TSharedRef<SButton>>		TabButtonMap;
};
