
#include "ClassBrowserTab.h"

#include <regex>

#include "Widgets/Views/STableRow.h"

using namespace ClassBrowser_Detail;

enum class SClassBrowserTab::EClassRecordListAction
{
	Jump,
	Clear,
	Add,
};


void SClassBrowserTab::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.3f)
		.MaxWidth(400)
		[
			SAssignNew(ClassNamePanel, SBorder)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			SAssignNew(ClassInfoPanel, SBorder)
		]

	];

	ClassNamePanel->Construct(
		SBorder::FArguments()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(0.1f)
			.MaxHeight(40)
			[
				SNew(SBorder)
				[
					SAssignNew(ClassNameSearchBox, SSearchBox)
					.OnTextChanged(SharedThis(this), &SClassBrowserTab::OnClassNameFilterChanged)
				]
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SBorder)
				[
					SAssignNew(ClassNameListView, SClassNameListView)
					.ListItemsSource(&ClassNameListItems)
					.OnGenerateRow(SharedThis(this), &SClassBrowserTab::OnGenerateWidgetForClassNameListView)
				]
			]	
		]
	);

	ClassInfoPanel->Construct(
		SBorder::FArguments()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(0.2f)
			.MaxHeight(60)
			[
				SNew(SBorder)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.AutoWidth()
					[
						SAssignNew(ClassRecordListView, SClassRecordListView)
						.Orientation(Orient_Horizontal)
						.ListItemsSource(&ClassRecordListItems)
						.OnGenerateRow(SharedThis(this), &SClassBrowserTab::OnGenerateWidgetForClassRecordListView)
					]
					
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.AutoWidth()
					.Padding(FMargin(5, 0))
					[
						SAssignNew(ShowSuperCheckBox, SCheckBox)
						.OnCheckStateChanged(SharedThis(this), &SClassBrowserTab::OnClickClassInfoCheckBox)
						[
							SNew(SBorder)
							// .BorderImage(&FClassStyle::Get()->BorderBackground)
							[
								SNew(STextBlock)
								.Text(FText::FromString("Show Parent"))
								.ColorAndOpacity(FClassStyle::Get()->GetColorByType(EClassType::Class))
							]
						]
					]
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.AutoWidth()
					.Padding(FMargin(5, 0))
					[
						SAssignNew(ShowPropertyCheckBox, SCheckBox)
						.OnCheckStateChanged(SharedThis(this), &SClassBrowserTab::OnClickClassInfoCheckBox)
						[
							SNew(SBorder)
							// .BorderImage(&FClassStyle::Get()->BorderBackground)
							[
								SNew(STextBlock)
								.Text(FText::FromString("Show Property"))
								.ColorAndOpacity(FClassStyle::Get()->GetColorByType(EClassType::Property))
							]
						]
					]
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.Padding(FMargin(5, 0))
					.AutoWidth()
					[
						SAssignNew(ShowFunctionCheckBox, SCheckBox)
						.OnCheckStateChanged(SharedThis(this), &SClassBrowserTab::OnClickClassInfoCheckBox)
						[
							SNew(SBorder)
							// .BorderImage(&FClassStyle::Get()->BorderBackground)
							[
								SNew(STextBlock)
								.Text(FText::FromString("Show Function"))
								.ColorAndOpacity(FClassStyle::Get()->GetColorByType(EClassType::Function))
							]
						]
					]
				]
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.MaxWidth(400)
				.FillWidth(0.3)
				[
					SNew(SBorder)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.FillHeight(0.1f)
						.MaxHeight(40)
						[
						SNew(SBorder)
						[
							SAssignNew(ClassInfoSearchBox, SSearchBox)
							.OnTextChanged(SharedThis(this), &SClassBrowserTab::OnClassInfoFilterChanged)
						]
						]
						+ SVerticalBox::Slot()
						.FillHeight(1.0f)
						[
							SNew(SBorder)
							[
								SAssignNew(ClassInfoListView, SClassInfoListView)
								.ListItemsSource(&ClassInfoListItems)
								.OnGenerateRow(SharedThis(this), &SClassBrowserTab::OnGenerateWidgetForClassInfoListView)
							]
						]
					]
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(FMargin(10, 0))
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.MaxHeight(30)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Right)
						.AutoWidth()
						[
							SAssignNew(ClassSourceCodeButton, SButton)
							.VAlign(VAlign_Center)
							.Text(FText::FromString(TEXT("Open Source Code")))
							.OnClicked(SharedThis(this), &SClassBrowserTab::OnClickClassSourceCodeButton)
						]
					]
					+ SVerticalBox::Slot()
					.FillHeight(1.0)
					[
						SNew(SBorder)
						[
							SNew(SScrollBox)
							.Orientation(Orient_Horizontal)
							+ SScrollBox::Slot()
							.Padding(FMargin(5, 5))
							[
								SNew(SScrollBox)
								.Orientation(Orient_Vertical)
								+ SScrollBox::Slot()
								[
									SAssignNew(DetailText, STextBlock)
								]
							]	
						]
					]
				]
			]
		]
	);

	FModuleManager::Get().OnModulesChanged().AddSP( this, &SClassBrowserTab::OnModuleChanged );

	UpdateClassNameListItems();

	ClassInfoPanel->SetVisibility(EVisibility::Hidden);
	ShowSuperCheckBox->SetIsChecked(ECheckBoxState::Checked);
	ShowPropertyCheckBox->SetIsChecked(ECheckBoxState::Checked);
	ShowFunctionCheckBox->SetIsChecked(ECheckBoxState::Checked);

	// add RichTextBlock Style
	auto& Style = static_cast<FSlateStyleSet&>(const_cast<ISlateStyle&>(FAppStyle::Get()));
	Style.Set(
		"ClassBrowser.HighlightFilter",
		FTextBlockStyle(Style.GetWidgetStyle<FTextBlockStyle>("NormalText"))
			.SetColorAndOpacity(FLinearColor(1.0, 0.0, 0.0))
		);
}


void SClassBrowserTab::OnModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	ClassNameListCache.Reset();
	UpdateClassNameListItems();
	UpdateCurrentClass();
}


void SClassBrowserTab::OnClassNameFilterChanged(const FText& InFilterText)
{
	UpdateClassNameListItems();
}

TSharedRef<ITableRow> SClassBrowserTab::OnGenerateWidgetForClassNameListView(TSharedPtr<FClassNameListItem> InItem,
	const TSharedRef<STableViewBase>& OwnerTable)
{

	class SClassNameItemWidget: public STableRow<TSharedPtr<FClassNameListItem>>
	{
	public:
		SLATE_BEGIN_ARGS(SClassNameItemWidget){}
		SLATE_END_ARGS()
		using FSuperRowType = STableRow<TSharedPtr<FClassNameListItem>>;
		
		void Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, TSharedPtr<FClassNameListItem> InListItem, TSharedRef<SClassBrowserTab> TabRef)
		{
			Item = InListItem;
			FSuperRowType::Construct(
				FSuperRowType::FArguments()
				.Padding(FMargin(2, 2))
				.Style(FClassStyle::Get()->TableRowStyle.Get())
				[
					SNew(SBox)
					.HeightOverride(32)
					[
						SNew(SButton)
						.VAlign(VAlign_Center)
						[
							SNew(SRichTextBlock)
							.DecoratorStyleSet(&FAppStyle::Get())
							.Text(FText::FromString(GenerateText(TabRef, Item->GetName())))
						]
						.OnClicked(FOnClicked::CreateLambda(
							[TabRef, ItemRef=Item.ToSharedRef()]
							{
								TabRef->OnClickClassName(ItemRef, EClassRecordListAction::Clear);
								if (TabRef->ClassNameListView.IsValid())
								{
									TabRef->ClassNameListView->SetSelection(ItemRef);
								}
								return FReply::Handled();
							}))
						]
				]
				, InOwnerTable);
		}

		FString GenerateText(TSharedRef<SClassBrowserTab> TabRef, const FString& InText)
		{
			const FString FilterStr = TabRef->ClassNameSearchBox->GetText().ToString();
			if(FilterStr.IsEmpty())
			{
				return InText;
			}
			
			FString Ret;
			int FilterIndex = 0; 
			for(const auto Char : InText)
			{
				if(FilterIndex < FilterStr.Len() && tolower(Char) == tolower(FilterStr[FilterIndex]))
				{
					Ret.Append("<ClassBrowser.HighlightFilter>");
					Ret.AppendChar(Char);
					Ret.Append("</>");
					++FilterIndex;
				}
				else
				{
					Ret.AppendChar(Char);
				}
			}
			return Ret;
		}

	private:
		TSharedPtr<FClassNameListItem> Item;
	};

	return SNew(SClassNameItemWidget, OwnerTable, InItem, SharedThis(this));
}

void SClassBrowserTab::UpdateClassNameListItems()
{
	ClassNameListItems.Reset();
	
	if (!ClassNameListCache.Num())
	{
		UpdateClassNameList();
	}

	const FString FilterStr = ClassNameSearchBox->GetText().ToString();
	std::wregex Pattern;
	
	if (!FilterStr.IsEmpty())
	{
		const FString PatternStr = TEXT(".*?") + FString::Join(FilterStr, TEXT(".*?")) + TEXT(".*?");
		Pattern	= std::wregex(ToCStr(PatternStr), std::regex::icase);
	}
	
	for (TSharedPtr<FClassNameListItem> Item: ClassNameListCache)
	{
		if (!FilterStr.IsEmpty())
		{
			if (!std::regex_match(ToCStr(Item->GetName()), Pattern)) continue;
		}
		ClassNameListItems.Push(Item);
	}

	if (ClassNameListView.IsValid())
	{
		ClassNameListView->RebuildList();
		ClassNameListView->RequestListRefresh();
		ClassNameListView->ScrollToTop();
	}
}

template <class Type, class NameType>
SClassBrowserTab::FClassNameList SClassBrowserTab::GetClasses()
{
	TArray<UObject*> Objects;
	GetObjectsOfClass(Type::StaticClass(), Objects);
	FClassNameList Result;
	Result.Reserve(Objects.Num());
	for(const UObject* Object: Objects)
	{
		const Type* Class = Cast<Type>(Object);
		TSharedRef<FClassNameListItem> NewItem(new NameType(Class));
		Result.Push(NewItem);
	}
	return Result;
}


void SClassBrowserTab::UpdateClassNameList()
{
	ClassNameListCache.Reset();

	ClassNameListCache.Append(GetClasses<UClass, FClassItemClass>());
	ClassNameListCache.Append(GetClasses<UEnum, FClassItemEnum>());
	ClassNameListCache.Append(GetClasses<UScriptStruct, FClassItemScriptStruct>());

	ClassNameListCache.Sort(
		[](TSharedPtr<FClassNameListItem> a, TSharedPtr<FClassNameListItem> b) -> bool
		{return a->GetName() < b->GetName();});
}

void SClassBrowserTab::OnClickClassName(const TSharedRef<FClassNameListItem> ItemRef, EClassRecordListAction Action) 
{
	auto Predicate = [This=SharedThis(this)](const TSharedPtr<FClassNameListItem>& Item){ return Item->GetPathName() == This->CurrentClassItem->GetPathName(); };
	switch (Action)
	{
	case EClassRecordListAction::Add:
		if (CurrentClassItem.IsValid())
		{
			const auto Index = ClassRecordListItems.IndexOfByPredicate(Predicate);
			if (Index != INDEX_NONE && Index < ClassRecordListItems.Num() - 1)
			{
				ClassRecordListItems.SetNum(Index + 1);
			}
		}
		ClassRecordListItems.Push(ItemRef);
		break;
	case EClassRecordListAction::Clear:
		ClassRecordListItems.Reset();
		ClassRecordListItems.Push(ItemRef);
		break;
	case EClassRecordListAction::Jump:
		break;
	default:
		break;
	}

	const TSharedPtr<FClassNameListItem> PreItem = CurrentClassItem;
	CurrentClassItem = ItemRef;
	UpdateCurrentClass();
	
	if (ClassRecordListView.IsValid())
	{
		ClassRecordListView->RebuildList();
		ClassRecordListView->SetSelection(ItemRef);
	}

	const FString PreName = PreItem.IsValid() ? PreItem->GetName() : TEXT("");
	// if (CurrentClassItem.IsValid() & PreName != CurrentClassItem->GetName())
	{
		OnClickClassInfoButton();
	}
}

void SClassBrowserTab::UpdateCurrentClass()
{
	EVisibility Visible = CurrentClassItem.IsValid() ? EVisibility::Visible : EVisibility::Hidden;
	ClassInfoPanel->SetVisibility(Visible);
	Visible = CurrentClassItem.IsValid() && CurrentClassItem->CanNavigateToSource() ? EVisibility::Visible : EVisibility::Collapsed;
	ClassSourceCodeButton->SetVisibility(Visible);
	UpdateClassInfoList();
	UpdateClassInfoListItems();
}


TSharedRef<ITableRow> SClassBrowserTab::OnGenerateWidgetForClassInfoListView(TSharedPtr<FClassInfoListItem> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	class SClassNameItemWidget: public STableRow<TSharedPtr<FClassInfoListItem>>
	{
	public:
		SLATE_BEGIN_ARGS(SClassNameItemWidget){}
		SLATE_END_ARGS()
		using FSuperRowType = STableRow<TSharedPtr<FClassInfoListItem>>;
		
		void Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, TSharedPtr<FClassInfoListItem> InListItem, TSharedRef<SClassBrowserTab> TabRef)
		{
			Item = InListItem;
			FSuperRowType::Construct(
				FSuperRowType::FArguments()
				.Padding(2)
				.Style(FClassStyle::Get()->TableRowStyle.Get())
				[
					SNew(SBox)
					.HeightOverride(32)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.FillWidth(1.0)
						[
							SNew(SButton)
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
								.Text(FText::FromString(Item->GetName()))
								.ColorAndOpacity(Item->GetColor())
							]
							.OnClicked(FOnClicked::CreateLambda(
							[TabRef, ItemRef=Item.ToSharedRef()]
							{
								TabRef->OnClickClassInfoItem(ItemRef);
								if (TabRef->ClassInfoListView.IsValid())
								{
									TabRef->ClassInfoListView->SetSelection(ItemRef);
								}
								return FReply::Handled();
							}))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SAssignNew(GotoClassButton, SButton)
							.Text(FText::FromString(TEXT(">>")))
							.Visibility(Item->GetType() == TabRef->CurrentClassItem->GetType() ? EVisibility::Visible : EVisibility::Collapsed)
							.OnClicked(FOnClicked::CreateLambda(
							[TabRef, ItemRef=Item.ToSharedRef()]
							{
								TabRef->OnClickClassName(ItemRef, EClassRecordListAction::Add);
								return FReply::Handled();
							}))	
						]
					]
				]
				, InOwnerTable);
		}

	private:
		TSharedPtr<FClassInfoListItem> Item;
		TSharedPtr<SButton> GotoClassButton;
	};

	return SNew(SClassNameItemWidget, OwnerTable, InItem, SharedThis(this));
}

void SClassBrowserTab::UpdateClassInfoList()
{
	ClassInfoListCache.Reset();
	if (CurrentClassItem.IsValid())
	{
		ClassInfoListCache.Append(CurrentClassItem->GetItemInfoList());
	}
}

void SClassBrowserTab::UpdateClassInfoListItems()
{
	ClassInfoListItems.Reset();
	
	if (!ClassInfoListCache.Num())
	{
		UpdateClassInfoList();
	}

	const FString FilterStr = ClassInfoSearchBox->GetText().ToString();
	std::wregex Pattern;
	
	if (!FilterStr.IsEmpty())
	{
		const FString PatternStr = TEXT(".*?") + FString::Join(FilterStr, TEXT(".*?")) + TEXT(".*?");
		Pattern	= std::wregex(ToCStr(PatternStr), std::regex::icase);
	}

	const bool ShowSuper = ShowSuperCheckBox->IsChecked();
	const bool ShowProperty = ShowPropertyCheckBox->IsChecked();
	const bool ShowFunction = ShowFunctionCheckBox->IsChecked();
	for (TSharedPtr<FClassInfoListItem> Item: ClassInfoListCache)
	{
		if (!ShowSuper && Item->GetType() == CurrentClassItem->GetType()) continue;
		if (!ShowProperty && Item->GetType() == EClassType::Property) continue;
		if (!ShowFunction && Item->GetType() == EClassType::Function) continue;
		if (!FilterStr.IsEmpty())
		{
			if (!std::regex_match(ToCStr(Item->GetName()), Pattern)) continue;
		}
		ClassInfoListItems.Push(Item);
	}

	if (ClassInfoListView.IsValid())
	{
		ClassInfoListView->RequestListRefresh();
		ClassInfoListView->ScrollToTop();
	}
}

void SClassBrowserTab::OnClassInfoFilterChanged(const FText& InFilterText)
{
	UpdateClassInfoListItems();
}


void SClassBrowserTab::OnClickClassInfoItem(const TSharedRef<FClassInfoListItem> ItemRef) const
{
	if (!CurrentClassItem.IsValid()) return;
	DetailText->SetText(FText::FromString(ItemRef->GetDesc()));
}


FReply SClassBrowserTab::OnClickClassInfoButton() const
{
	if (!CurrentClassItem.IsValid()) return FReply::Handled();
	DetailText->SetText(FText::FromString(CurrentClassItem->GetDesc()));
	return FReply::Handled();
}

FReply SClassBrowserTab::OnClickClassSourceCodeButton() const
{
	if (!CurrentClassItem.IsValid()) return FReply::Handled();
	CurrentClassItem->NavigateToSource();
	return FReply::Handled();
}

void SClassBrowserTab::OnClickClassInfoCheckBox(ECheckBoxState CheckState)
{
	UpdateClassInfoListItems();
}


TSharedRef<ITableRow> SClassBrowserTab::OnGenerateWidgetForClassRecordListView(TSharedPtr<FClassNameListItem> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	class SClassNameItemWidget: public STableRow<TSharedPtr<FClassNameListItem>>
	{
	public:
		SLATE_BEGIN_ARGS(SClassNameItemWidget){}
		SLATE_END_ARGS()
		using FSuperRowType = STableRow<TSharedPtr<FClassNameListItem>>;
		
		void Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, TSharedPtr<FClassNameListItem> InListItem, TSharedRef<SClassBrowserTab> TabRef)
		{
			Item = InListItem;
			TabWeakRef = TabRef;
			FSuperRowType::Construct(
				FSuperRowType::FArguments()
				.Padding(FMargin(0, 2))
				.Style(FClassStyle::Get()->TableRowStyle.Get())
				[
					SAssignNew(Border, SBorder)
					[
						SAssignNew(Button, SButton)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString(Item->GetName()))
						]
						.OnClicked(FOnClicked::CreateLambda(
							[TabRef, ItemRef=Item.ToSharedRef()]
							{
								TabRef->OnClickClassName(ItemRef, EClassRecordListAction::Jump);
								return FReply::Handled();
							}))
					]
				]
				, InOwnerTable);
		}
	private:
		TSharedPtr<FClassNameListItem> Item;
		TSharedPtr<SButton> Button;
		TSharedPtr<SBorder> Border;
		TWeakPtr<SClassBrowserTab> TabWeakRef;
	};

	return SNew(SClassNameItemWidget, OwnerTable, InItem, SharedThis(this));	
}
