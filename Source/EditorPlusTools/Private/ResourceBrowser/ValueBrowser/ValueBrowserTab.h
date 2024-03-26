
#pragma once

#include "ValueInfo.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/STileView.h>

class SMultiLineEditableText;

class SValueBrowserTab final : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SValueBrowserTab)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void OnClose() {};

	using FValueType = TSharedRef<FValueInfo>;
	using FValueListType = TArray<FValueType>;

private:
	TSharedRef<SWidget> ConstructContent();
	TSharedRef<ITableRow> OnValueTile(FValueType InItem, const TSharedRef<STableViewBase>& OwnerTable);
	FReply OnClickValue(const FValueType& InItem);
	void OnHoverValue(const FValueType& InItem);
	void OnUnhoverValue(const FValueType& InItem);
	FReply OnResetValue();
	FReply OnSelectValue(const FString& InAction, const FValueType& InValue);

	void OnSearchValue(const FText& InFilterText);
	void UpdateValueList();

private:
	FValueListType					ValueAllList;
	FValueListType					ValueList;
	typedef STileView<FValueType>	SValueView;
	TSharedPtr<SValueView>			ValueView;

	TSharedPtr<SSearchBox>			ValueSearchBox;
	TSharedPtr<FActiveTimerHandle>	ValueSearchTimer;
	const float						ValueSearchDelay = 0.1f;

	// usage
	TSharedPtr<FValueInfo>			SelectedValue;
	TSharedPtr<SMultiLineEditableText>		SelectedValueName;
	TSharedPtr<SMultiLineEditableText>		DetailText;
	TSharedPtr<SMultiLineEditableText>		DetailValue;
	TSharedPtr<SMultiLineEditableText>		DemonstrateText;
	FText									DemonstrateContent;
};
