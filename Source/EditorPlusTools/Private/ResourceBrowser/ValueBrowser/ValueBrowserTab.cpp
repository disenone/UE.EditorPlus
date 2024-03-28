
#include "ValueBrowserTab.h"
#include "EditorPlusUtils.h"

#include <regex>
#include <Widgets/Input/SSearchBox.h>
#include <WIdgets/Text/SMultiLineEditableText.h>

const static FString DefaultValueDetail("Click Value to Select One\n[Total Num]: {0}");
const static FText DefaultValueName = FText::FromString("No Value is Selected");

void SValueBrowserTab::Construct(const FArguments& InArgs)
{
	ValueList = ValueAllList = *FValueInfo::CollectValues();

	ChildSlot
	[
		ConstructContent()
	];

}

TSharedRef<SWidget> SValueBrowserTab::ConstructContent()
{
	return SNew(SVerticalBox)
		// search
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			[
				SAssignNew(ValueSearchBox, SSearchBox)
				.OnTextChanged(this, &SValueBrowserTab::OnSearchValue)
			]
		]
		// Value tile view
		+ SVerticalBox::Slot()
		.FillHeight(0.8f)
		[
			SAssignNew(ValueView, SValueView)
			.ListItemsSource(&ValueList)
			.OnGenerateTile(this, &SValueBrowserTab::OnValueTile)

		]
		// detail
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)[
			SNew(SBox)
			.HeightOverride(128)
			[
				SNew(SHorizontalBox)
				// Value and reset button
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(128)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SBox)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							.HeightOverride(72)
							[
								SNew(SBorder)
								[
									SNew(SBox)
									.HeightOverride(64)
									.WidthOverride(64)
									[
										SAssignNew(DetailValue, SMultiLineEditableText)
										.AutoWrapText(true)
										.AllowMultiLine(true)
										.IsReadOnly(true)
										.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
										.AllowContextMenu(false)
									]
								]
							]
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SBox)
							.HeightOverride(32)
							[
								SAssignNew(SelectedValueName, SMultiLineEditableText)
								.AutoWrapText(true)
								.AllowMultiLine(true)
								.IsReadOnly(true)
								.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
								.Text(DefaultValueName)
							]

						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SBox)
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.Text(FText::FromString("Reset"))
								.OnClicked(this, &SValueBrowserTab::OnResetValue)
							]
						]
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(3.0f)
					[SNew(SBorder)]
				]

				// detail text
				+ SHorizontalBox::Slot()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.FillHeight(0.5f)
					[
						SAssignNew(DetailText, SMultiLineEditableText)
						.AutoWrapText(true)
						.AllowMultiLine(true)
						.IsReadOnly(true)
						.AllowContextMenu(true)
						.Text(FText::FromString(FString::Format(*DefaultValueDetail, {ValueAllList.Num()})))
					]
				]
			]]
		]
	;
}

TSharedRef<ITableRow> SValueBrowserTab::OnValueTile(FValueType InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(STableRow< TSharedPtr<FString> >, OwnerTable)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(SBorder)
					[
						SNew(SBox)
						.HeightOverride(64)
						.WidthOverride(64)
						[
							SNew(STextBlock)
							.AutoWrapText(true)
							.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
							.Text(FText::FromString(InItem->ValueName))
						]
					]
				]
				.OnClicked_Lambda([this, InItem]()
				{
					return this->OnClickValue(InItem);
				})
				.OnHovered_Lambda([this, InItem]()
				{
					return this->OnHoverValue(InItem);
				})
				.OnUnhovered_Lambda([this, InItem]()
				{
					return this->OnUnhoverValue(InItem);
				})
			]
			+ SVerticalBox::Slot()
			.FillHeight(0.3)
			[
				SNew(SMultiLineEditableText)
				.AutoWrapText(true)
				.AllowMultiLine(true)
				.IsReadOnly(true)
				.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
				.Text(FText::FromString(InItem->SimpleName))
			]
		];
}

void SValueBrowserTab::OnSearchValue(const FText& InFilterText)
{
	if (ValueSearchTimer.IsValid())
	{
		UnRegisterActiveTimer(ValueSearchTimer.ToSharedRef());
	}

	ValueSearchTimer = RegisterActiveTimer(ValueSearchDelay, FWidgetActiveTimerDelegate::CreateLambda(
	[self = SharedThis(this)](double, double)
	{
		self->ValueSearchTimer.Reset();
		self->UpdateValueList();
		return EActiveTimerReturnType::Stop;
	}));
}

void SValueBrowserTab::UpdateValueList()
{
	const FString FilterStr = ValueSearchBox->GetText().ToString();

	if (FilterStr.IsEmpty())
	{
		if (ValueList.Num() != ValueAllList.Num())
			ValueList = ValueAllList;
	}
	else
	{
		ValueList.Empty();

		const FString PatternStr = TEXT("(.*?)(") + FString::Join(FEditorPlusUtils::SplitString(FilterStr, " "), TEXT(".*?")) + TEXT(")(.*)");
		const std::wregex Pattern = std::wregex(TCHAR_TO_WCHAR(ToCStr(PatternStr)), std::regex::icase);
		using FMatchItemType = TTuple<uint16, uint16, TSharedPtr<FValueInfo>>;
		TArray<FMatchItemType> MatchItems;

		for (TSharedRef<FValueInfo> Item: ValueAllList)
		{
			std::wsmatch MatchResult;
			const std::wstring SearchText(TCHAR_TO_WCHAR(ToCStr(Item->FriendlyName)));
			if (!std::regex_match(SearchText, MatchResult, Pattern)) continue;

			if (MatchResult.empty() || !MatchResult.ready()) continue;

			MatchItems.Push(MakeTuple<uint16, uint16, TSharedPtr<FValueInfo>>(
				MatchResult[1].length(), MatchResult[2].length(), Item));
		}

		if (!MatchItems.IsEmpty())
		{
			MatchItems.Sort([](const FMatchItemType& A, const FMatchItemType& B)
			{
				if (A.Get<0>() != B.Get<0>())
				{
					return A.Get<0>() < B.Get<0>();
				}

				if (A.Get<1>() != B.Get<1>())
				{
					return A.Get<1>() < B.Get<1>();
				}
				return *A.Get<2>() < *B.Get<2>();
			});
			for (FMatchItemType& MatchItem: MatchItems)
			{
				ValueList.Push(MatchItem.Get<2>().ToSharedRef());
			}
		}
	}

	if (ValueView.IsValid())
	{
		ValueView->RebuildList();
		ValueView->RequestListRefresh();
		ValueView->ScrollToTop();
	}
}

FString BuildDetail(const FString& Action, const SValueBrowserTab::FValueType& InValue)
{
	FString Usage = InValue->Usage.Replace(TEXT("\n"), TEXT(" "));

	return FString::Format(
		TEXT(
			"[{0}]: {1}\n"
			"[Value]: {2}\n"
			"[Usage]: {3}\n"),
		{
			Action,
			InValue->FriendlyName,
			InValue->ValueName,
			Usage,
			}
		);
}

FReply SValueBrowserTab::OnClickValue(const FValueType& InItem)
{
	if (SelectedValue == InItem)
	{
		return OnResetValue();
	}

	SelectedValue = InItem;
	return OnSelectValue("Clicked", InItem);
}

void SValueBrowserTab::OnHoverValue(const FValueType& InItem)
{
	if (!SelectedValue.IsValid())
	{
		OnSelectValue("Hovered", InItem);
	}
}

void SValueBrowserTab::OnUnhoverValue(const FValueType& InItem)
{
	if (!SelectedValue.IsValid())
	{
		OnResetValue();
	}
}

FReply SValueBrowserTab::OnResetValue()
{
	SelectedValue.Reset();

	SelectedValueName->SetText(DefaultValueName);
	DetailValue->SetText(FText::FromString(""));
	DetailText->SetText(FText::FromString(FString::Format(*DefaultValueDetail, {ValueAllList.Num()})));

	return FReply::Handled();
}

FReply SValueBrowserTab::OnSelectValue(const FString& InAction, const FValueType& InValue)
{
	SelectedValueName->SetText(FText::FromString(InValue->SimpleName));
	DetailValue->SetText(FText::FromString(InValue->ValueName));
	DetailText->SetText(FText::FromString(BuildDetail(InAction, InValue)));

	return FReply::Handled();
}
