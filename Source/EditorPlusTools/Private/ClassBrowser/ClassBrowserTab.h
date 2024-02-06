// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once

#include "ClassItem.h"
#include <Widgets/SCompoundWidget.h>

class SMultiLineEditableText;

class SClassBrowserTab final : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SClassBrowserTab)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void OnClose();
	
private:
	
	// class name view
	using FClassNameListItem = ClassBrowser_Detail::FClassItemBase;
	using FClassNameList = TArray<TSharedPtr<FClassNameListItem>>;
	enum class EClassRecordListAction;
	TSharedRef<ITableRow> OnGenerateWidgetForClassNameListView(TSharedPtr<FClassNameListItem> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void UpdateClassNameList();
	void UpdateClassNameListItems();
	void OnModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange);
	void OnClickClassName(const TSharedRef<FClassNameListItem>& ItemRef, const EClassRecordListAction Action);
	void OnClassNameFilterChanged(const FText& InFilterText);
	template <class Type, class NameType> FClassNameList GetClasses();

	// class info view
	using FClassInfoListItem = ClassBrowser_Detail::FClassItemBase;
	TSharedRef<ITableRow> OnGenerateWidgetForClassInfoListView(TSharedPtr<FClassInfoListItem> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void UpdateClassInfoList();
	void UpdateClassInfoListItems();
	void OnClickClassInfoItem(const TSharedRef<FClassInfoListItem>& ItemRef) const;
	FReply OnClickClassInfoButton() const;
	void OnClassInfoFilterChanged(const FText& InFilterText);
	void UpdateCurrentClass();
	FReply OnClickClassSourceCodeButton() const;
	void OnClickClassInfoCheckBox(ECheckBoxState CheckState);

	// class record
	TSharedRef<ITableRow> OnGenerateWidgetForClassRecordListView(TSharedPtr<FClassNameListItem> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	// detail font
	void OnDetailFontSizeChange(uint8 Size, ETextCommit::Type CommitType);
	TOptional<uint8> GetDetailFontSize() const;
	
private:
	// class name view
	TSharedPtr<SSearchBox>	ClassNameSearchBox;
	TSharedPtr<SBorder> ClassNamePanel;

	typedef SListView<TSharedPtr<FClassNameListItem>> SClassNameListView;
	TSharedPtr<SClassNameListView>	ClassNameListView;
	FClassNameList ClassNameListCache;
	FClassNameList ClassNameListItems;
	TSharedPtr<FActiveTimerHandle> ClassNameSearchTimer;

	// class info view
	TSharedPtr<FClassNameListItem> CurrentClassItem;
	TSharedPtr<SButton>	ClassSourceCodeButton;
	TSharedPtr<SSearchBox>	ClassInfoSearchBox;
	TSharedPtr<SBorder> ClassInfoPanel;
	
	typedef SListView<TSharedPtr<FClassInfoListItem>> SClassInfoListView;
	TSharedPtr<SClassInfoListView> ClassInfoListView;
	TArray<TSharedPtr<FClassInfoListItem>> ClassInfoListCache;
	TArray<TSharedPtr<FClassInfoListItem>> ClassInfoListItems;
	TSharedPtr<FActiveTimerHandle> ClassInfoSearchTimer;

	TSharedPtr<SCheckBox> ShowSuperCheckBox;
	TSharedPtr<SCheckBox> ShowPropertyCheckBox;
	TSharedPtr<SCheckBox> ShowFunctionCheckBox;
	
	// detail
	TSharedPtr<SVerticalBox>	DetailPanel;
	TSharedPtr<SMultiLineEditableText>		DetailText;
	uint8						DetailFontSize=10;

	// class record
	typedef SListView<TSharedPtr<FClassNameListItem>> SClassRecordListView;
	TSharedPtr<SClassRecordListView> ClassRecordListView;
	FClassNameList ClassRecordListItems;

	const float SearchDelay = 0.1f;
};
