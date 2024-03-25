
#include "ColorBrowserTab.h"
#include "EditorPlusUtils.h"

#include <regex>
#include <Widgets/Input/SSearchBox.h>

#include "WIdgets/Text/SMultiLineEditableText.h"

const static FString DefaultColorDetail("Click Color to Select One\n[Total Num]: {0}");
const static FText DefaultColorName = FText::FromString("No Color is Selected");

void SColorBrowserTab::Construct(const FArguments& InArgs)
{
	ColorAllList = *FColorInfo::CollectColors();

	ChildSlot
	[
		ConstructContent()
	];

}

TSharedRef<SWidget> SColorBrowserTab::ConstructContent()
{
	return SNew(SVerticalBox)
		// search
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			[
				SAssignNew(ColorSearchBox, SSearchBox)
				.OnTextChanged(this, &SColorBrowserTab::OnSearchColor)
			]
		]
		// Color tile view
		+ SVerticalBox::Slot()
		.FillHeight(0.8f)
		[
			SAssignNew(ColorView, SColorView)
			.ListItemsSource(&ColorAllList)
			.OnGenerateTile(this, &SColorBrowserTab::OnColorTile)

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
				// Color and reset button
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
									SAssignNew(DetailColor, SImage)
									.DesiredSizeOverride(FVector2d(64, 64))
									.Image(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Default").GetIcon())
								]
							]
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SBox)
							.HeightOverride(32)
							[
								SAssignNew(SelectedColorName, SMultiLineEditableText)
								.AutoWrapText(true)
								.AllowMultiLine(true)
								.IsReadOnly(true)
								.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
								.Text(DefaultColorName)
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
								.OnClicked(this, &SColorBrowserTab::OnResetColor)
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
					SNew(SBox)
					.Padding(2.0f, 8.0f)
					[
						SAssignNew(DetailText, SMultiLineEditableText)
						.AutoWrapText(true)
						.AllowMultiLine(true)
						.IsReadOnly(true)
						.AllowContextMenu(true)
						.Text(FText::FromString(FString::Format(*DefaultColorDetail, {ColorAllList.Num()})))
					]
				]
			]]
		]
	;
}

TSharedRef<ITableRow> SColorBrowserTab::OnColorTile(FColorType InItem, const TSharedRef<STableViewBase>& OwnerTable)
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
						SNew(SImage)
						// .Image(InItem->Color.GetColor())
						.DesiredSizeOverride(FVector2d(64, 64))
						.ColorAndOpacity(InItem->Color)
					]
				]
				.OnClicked_Lambda([this, InItem]()
				{
					return this->OnClickColor(InItem);
				})
				.OnHovered_Lambda([this, InItem]()
				{
					return this->OnHoverColor(InItem);
				})
				.OnUnhovered_Lambda([this, InItem]()
				{
					return this->OnUnhoverColor(InItem);
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

void SColorBrowserTab::OnSearchColor(const FText& InFilterText)
{
	if (ColorSearchTimer.IsValid())
	{
		UnRegisterActiveTimer(ColorSearchTimer.ToSharedRef());
	}

	ColorSearchTimer = RegisterActiveTimer(ColorSearchDelay, FWidgetActiveTimerDelegate::CreateLambda(
	[self = SharedThis(this)](double, double)
	{
		self->ColorSearchTimer.Reset();
		self->UpdateColorList();
		return EActiveTimerReturnType::Stop;
	}));
}

void SColorBrowserTab::UpdateColorList()
{
	const FString FilterStr = ColorSearchBox->GetText().ToString();

	if (FilterStr.IsEmpty())
	{
		ColorView->SetItemsSource(&ColorAllList);
	}
	else
	{
		ColorList.Empty();
		ColorView->SetItemsSource(&ColorList);

		const FString PatternStr = TEXT("(.*?)(") + FString::Join(FEditorPlusUtils::SplitString(FilterStr, " "), TEXT(".*?")) + TEXT(")(.*)");
		const std::wregex Pattern = std::wregex(TCHAR_TO_WCHAR(ToCStr(PatternStr)), std::regex::icase);
		using FMatchItemType = TTuple<uint16, uint16, TSharedPtr<FColorInfo>>;
		TArray<FMatchItemType> MatchItems;

		for (TSharedRef<FColorInfo> Item: ColorAllList)
		{
			std::wsmatch MatchResult;
			const std::wstring SearchText(TCHAR_TO_WCHAR(ToCStr(Item->FriendlyName)));
			if (!std::regex_match(SearchText, MatchResult, Pattern)) continue;

			if (MatchResult.empty() || !MatchResult.ready()) continue;

			MatchItems.Push(MakeTuple<uint16, uint16, TSharedPtr<FColorInfo>>(
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
				ColorList.Push(MatchItem.Get<2>().ToSharedRef());
			}
		}
	}

	if (ColorView.IsValid())
	{
		ColorView->RebuildList();
		ColorView->RequestListRefresh();
		ColorView->ScrollToTop();
	}
}

FString BuildDetail(const FString& Action, const SColorBrowserTab::FColorType& InColor)
{
	FString Usage = InColor->Usage.Replace(TEXT("\n"), TEXT(" "));

	FLinearColor Color = InColor->Color.GetColor(FWidgetStyle());

	return FString::Format(
		TEXT(
			"[{0}]: {1}\n"
			"[RGBA]: {2}\n"
			"[Usage]: {3}\n"),
		{
			Action,
			InColor->FriendlyName,
			InColor->Color.GetColor(FWidgetStyle()).ToString(),
			Usage,
			}
		);
}

FReply SColorBrowserTab::OnClickColor(const FColorType& InItem)
{
	if (SelectedColor == InItem)
	{
		return OnResetColor();
	}

	SelectedColor = InItem;
	return OnSelectColor("Clicked", InItem);
}

void SColorBrowserTab::OnHoverColor(const FColorType& InItem)
{
	if (!SelectedColor.IsValid())
	{
		OnSelectColor("Hovered", InItem);
	}
}

void SColorBrowserTab::OnUnhoverColor(const FColorType& InItem)
{
	if (!SelectedColor.IsValid())
	{
		OnResetColor();
	}
}

FReply SColorBrowserTab::OnResetColor()
{
	SelectedColor.Reset();

	SelectedColorName->SetText(DefaultColorName);
	DetailColor->SetColorAndOpacity(FLinearColor::White);
	DetailText->SetText(FText::FromString(FString::Format(*DefaultColorDetail, {ColorAllList.Num()})));

	return FReply::Handled();
}

FReply SColorBrowserTab::OnSelectColor(const FString& InAction, const FColorType& InColor)
{
	SelectedColorName->SetText(FText::FromString(InColor->SimpleName));
	DetailColor->SetColorAndOpacity(InColor->Color);
	DetailText->SetText(FText::FromString(BuildDetail(InAction, InColor)));

	return FReply::Handled();
}
