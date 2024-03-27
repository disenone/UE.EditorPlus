
#include "WidgetStyleBrowserTab.h"
#include "EditorPlusUtils.h"

#include <regex>
#include <Widgets/Input/SSearchBox.h>
#include <WIdgets/Text/SMultiLineEditableText.h>

#include "IDetailTreeNode.h"

const static FString DefaultWidgetStyleDetail("Click WidgetStyle to Select One\n[Total Num]: {0}");
const static FText DefaultWidgetStyleName = FText::FromString("No WidgetStyle is Selected");

void SWidgetStyleBrowserTab::Construct(const FArguments& InArgs)
{
	WidgetStyleAllList = *FWidgetStyleInfo::CollectWidgetStyles();

	ChildSlot
	[
		ConstructContent()
	];

}

TSharedRef<SWidget> SWidgetStyleBrowserTab::ConstructContent()
{
	return SNew(SVerticalBox)
		// search
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			[
				SAssignNew(WidgetStyleSearchBox, SSearchBox)
				.OnTextChanged(this, &SWidgetStyleBrowserTab::OnSearchWidgetStyle)
			]
		]
		// WidgetStyle tile view
		+ SVerticalBox::Slot()
		.FillHeight(0.8f)
		[
			SAssignNew(WidgetStyleView, SWidgetStyleView)
			.ListItemsSource(&WidgetStyleAllList)
			.OnGenerateTile(this, &SWidgetStyleBrowserTab::OnWidgetStyleTile)

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
				// WidgetStyle and reset button
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
										SAssignNew(DetailWidgetStyle, SMultiLineEditableText)
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
								SAssignNew(SelectedWidgetStyleName, SMultiLineEditableText)
								.AutoWrapText(true)
								.AllowMultiLine(true)
								.IsReadOnly(true)
								.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
								.Text(DefaultWidgetStyleName)
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
								.OnClicked(this, &SWidgetStyleBrowserTab::OnResetWidgetStyle)
							]
						]
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(3.0f)
					[SNew(SSeparator).SeparatorImage(FCoreStyle::Get().GetBrush( "Border" ))]
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
						.Text(FText::FromString(FString::Format(*DefaultWidgetStyleDetail, {WidgetStyleAllList.Num()})))
					]
				]
			]]
		]
	;
}

TSharedRef<ITableRow> SWidgetStyleBrowserTab::OnWidgetStyleTile(FWidgetStyleType InItem, const TSharedRef<STableViewBase>& OwnerTable)
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
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							InItem->GetSampleWidget()
						]
					]
				]
				.OnClicked_Lambda([this, InItem]()
				{
					return this->OnClickWidgetStyle(InItem);
				})
				.OnHovered_Lambda([this, InItem]()
				{
					return this->OnHoverWidgetStyle(InItem);
				})
				.OnUnhovered_Lambda([this, InItem]()
				{
					return this->OnUnhoverWidgetStyle(InItem);
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

void SWidgetStyleBrowserTab::OnSearchWidgetStyle(const FText& InFilterText)
{
	if (WidgetStyleSearchTimer.IsValid())
	{
		UnRegisterActiveTimer(WidgetStyleSearchTimer.ToSharedRef());
	}

	WidgetStyleSearchTimer = RegisterActiveTimer(WidgetStyleSearchDelay, FWidgetActiveTimerDelegate::CreateLambda(
	[self = SharedThis(this)](double, double)
	{
		self->WidgetStyleSearchTimer.Reset();
		self->UpdateWidgetStyleList();
		return EActiveTimerReturnType::Stop;
	}));
}

void SWidgetStyleBrowserTab::UpdateWidgetStyleList()
{
	const FString FilterStr = WidgetStyleSearchBox->GetText().ToString();

	if (FilterStr.IsEmpty())
	{
		WidgetStyleView->SetItemsSource(&WidgetStyleAllList);
	}
	else
	{
		WidgetStyleList.Empty();
		WidgetStyleView->SetItemsSource(&WidgetStyleList);

		const FString PatternStr = TEXT("(.*?)(") + FString::Join(FEditorPlusUtils::SplitString(FilterStr, " "), TEXT(".*?")) + TEXT(")(.*)");
		const std::wregex Pattern = std::wregex(TCHAR_TO_WCHAR(ToCStr(PatternStr)), std::regex::icase);
		using FMatchItemType = TTuple<uint16, uint16, TSharedPtr<FWidgetStyleInfo>>;
		TArray<FMatchItemType> MatchItems;

		for (TSharedRef<FWidgetStyleInfo> Item: WidgetStyleAllList)
		{
			std::wsmatch MatchResult;
			const std::wstring SearchText(TCHAR_TO_WCHAR(ToCStr(Item->FriendlyName)));
			if (!std::regex_match(SearchText, MatchResult, Pattern)) continue;

			if (MatchResult.empty() || !MatchResult.ready()) continue;

			MatchItems.Push(MakeTuple<uint16, uint16, TSharedPtr<FWidgetStyleInfo>>(
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
				WidgetStyleList.Push(MatchItem.Get<2>().ToSharedRef());
			}
		}
	}

	if (WidgetStyleView.IsValid())
	{
		WidgetStyleView->RebuildList();
		WidgetStyleView->RequestListRefresh();
		WidgetStyleView->ScrollToTop();
	}
}

FString BuildDetail(const FString& Action, const SWidgetStyleBrowserTab::FWidgetStyleType& InWidgetStyle)
{
	FString Usage = InWidgetStyle->Usage.Replace(TEXT("\n"), TEXT(" "));

	return FString::Format(
		TEXT(
			"[{0}]: {1}\n"
			"[Usage]: {2}\n"),
		{
			Action,
			InWidgetStyle->FriendlyName,
			Usage,
			}
		);
}

FReply SWidgetStyleBrowserTab::OnClickWidgetStyle(const FWidgetStyleType& InItem)
{
	if (SelectedWidgetStyle == InItem)
	{
		return OnResetWidgetStyle();
	}

	SelectedWidgetStyle = InItem;
	return OnSelectWidgetStyle("Clicked", InItem);
}

void SWidgetStyleBrowserTab::OnHoverWidgetStyle(const FWidgetStyleType& InItem)
{
	if (!SelectedWidgetStyle.IsValid())
	{
		OnSelectWidgetStyle("Hovered", InItem);
	}
}

void SWidgetStyleBrowserTab::OnUnhoverWidgetStyle(const FWidgetStyleType& InItem)
{
	if (!SelectedWidgetStyle.IsValid())
	{
		OnResetWidgetStyle();
	}
}

FReply SWidgetStyleBrowserTab::OnResetWidgetStyle()
{
	SelectedWidgetStyle.Reset();

	SelectedWidgetStyleName->SetText(DefaultWidgetStyleName);
	DetailWidgetStyle->SetText(FText::FromString(""));
	DetailText->SetText(FText::FromString(FString::Format(*DefaultWidgetStyleDetail, {WidgetStyleAllList.Num()})));

	return FReply::Handled();
}

FReply SWidgetStyleBrowserTab::OnSelectWidgetStyle(const FString& InAction, const FWidgetStyleType& InWidgetStyle)
{
	SelectedWidgetStyleName->SetText(FText::FromString(InWidgetStyle->SimpleName));
	DetailWidgetStyle->SetText(FText::FromName(InWidgetStyle->WidgetTypeName));
	DetailText->SetText(FText::FromString(BuildDetail(InAction, InWidgetStyle)));

	return FReply::Handled();
}
