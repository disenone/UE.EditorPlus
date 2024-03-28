
#include "FontBrowserTab.h"
#include "EditorPlusUtils.h"

#include <regex>
#include <Widgets/Input/SSearchBox.h>
#include <WIdgets/Text/SMultiLineEditableText.h>

const static FString DefaultFontDetail("Click Font to Select One\n[Total Num]: {0}");
const static FText DefaultFontName = FText::FromString("No Font is Selected");
const static FText DefaultFontDemonstrate = FText::FromString("Sample text, you can modify this to anything you want.");

void SFontBrowserTab::Construct(const FArguments& InArgs)
{
	FontList = FontAllList = *FFontInfo::CollectFonts();

	ChildSlot
	[
		ConstructContent()
	];

}

TSharedRef<SWidget> SFontBrowserTab::ConstructContent()
{
	return SNew(SVerticalBox)
		// search
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			[
				SAssignNew(FontSearchBox, SSearchBox)
				.OnTextChanged(this, &SFontBrowserTab::OnSearchFont)
			]
		]
		// Font tile view
		+ SVerticalBox::Slot()
		.FillHeight(0.8f)
		[
			SAssignNew(FontView, SFontView)
			.ListItemsSource(&FontList)
			.OnGenerateTile(this, &SFontBrowserTab::OnFontTile)

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
				// Font and reset button
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
										SAssignNew(DetailFont, SMultiLineEditableText)
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
								SAssignNew(SelectedFontName, SMultiLineEditableText)
								.AutoWrapText(true)
								.AllowMultiLine(true)
								.IsReadOnly(true)
								.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
								.Text(DefaultFontName)
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
								.OnClicked(this, &SFontBrowserTab::OnResetFont)
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
						.Text(FText::FromString(FString::Format(*DefaultFontDetail, {FontAllList.Num()})))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.WidthOverride(3.0f)
						[SNew(SBorder)]
					]
					+ SVerticalBox::Slot()
					.FillHeight(0.5f)
					[
						SAssignNew(DemonstrateText, SMultiLineEditableText)
						.AutoWrapText(true)
						.AllowMultiLine(true)
						.IsReadOnly(false)
						.AllowContextMenu(true)
						.Text(DefaultFontDemonstrate)
						.OnTextChanged(this, &SFontBrowserTab::OnDemonstrateChange)
					]
				]
			]]
		]
	;
}

TSharedRef<ITableRow> SFontBrowserTab::OnFontTile(FFontType InItem, const TSharedRef<STableViewBase>& OwnerTable)
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
							.Text(FText::FromName(InItem->TypefaceFontName))
							.Font(InItem->Font)
						]
					]
				]
				.OnClicked_Lambda([this, InItem]()
				{
					return this->OnClickFont(InItem);
				})
				.OnHovered_Lambda([this, InItem]()
				{
					return this->OnHoverFont(InItem);
				})
				.OnUnhovered_Lambda([this, InItem]()
				{
					return this->OnUnhoverFont(InItem);
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

void SFontBrowserTab::OnSearchFont(const FText& InFilterText)
{
	if (FontSearchTimer.IsValid())
	{
		UnRegisterActiveTimer(FontSearchTimer.ToSharedRef());
	}

	FontSearchTimer = RegisterActiveTimer(FontSearchDelay, FWidgetActiveTimerDelegate::CreateLambda(
	[self = SharedThis(this)](double, double)
	{
		self->FontSearchTimer.Reset();
		self->UpdateFontList();
		return EActiveTimerReturnType::Stop;
	}));
}

void SFontBrowserTab::UpdateFontList()
{
	const FString FilterStr = FontSearchBox->GetText().ToString();

	if (FilterStr.IsEmpty())
	{
		if (FontList.Num() != FontAllList.Num())
			FontList = FontAllList;
	}
	else
	{
		FontList.Empty();

		const FString PatternStr = TEXT("(.*?)(") + FString::Join(FEditorPlusUtils::SplitString(FilterStr, " "), TEXT(".*?")) + TEXT(")(.*)");
		const std::wregex Pattern = std::wregex(TCHAR_TO_WCHAR(ToCStr(PatternStr)), std::regex::icase);
		using FMatchItemType = TTuple<uint16, uint16, TSharedPtr<FFontInfo>>;
		TArray<FMatchItemType> MatchItems;

		for (TSharedRef<FFontInfo> Item: FontAllList)
		{
			std::wsmatch MatchResult;
			const std::wstring SearchText(TCHAR_TO_WCHAR(ToCStr(Item->FriendlyName)));
			if (!std::regex_match(SearchText, MatchResult, Pattern)) continue;

			if (MatchResult.empty() || !MatchResult.ready()) continue;

			MatchItems.Push(MakeTuple<uint16, uint16, TSharedPtr<FFontInfo>>(
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
				FontList.Push(MatchItem.Get<2>().ToSharedRef());
			}
		}
	}

	if (FontView.IsValid())
	{
		FontView->RebuildList();
		FontView->RequestListRefresh();
		FontView->ScrollToTop();
	}
}

FString BuildDetail(const FString& Action, const SFontBrowserTab::FFontType& InFont)
{
	FString Usage = InFont->Usage.Replace(TEXT("\n"), TEXT(" "));

	return FString::Format(
		TEXT(
			"[{0}]: {1}\n"
			"[Font]: {2}\n"
			"[Usage]: {3}\n"),
		{
			Action,
			InFont->FriendlyName,
			InFont->TypefaceFontName.ToString(),
			Usage,
			}
		);
}

FReply SFontBrowserTab::OnClickFont(const FFontType& InItem)
{
	if (SelectedFont == InItem)
	{
		return OnResetFont();
	}

	SelectedFont = InItem;
	return OnSelectFont("Clicked", InItem);
}

void SFontBrowserTab::OnHoverFont(const FFontType& InItem)
{
	if (!SelectedFont.IsValid())
	{
		OnSelectFont("Hovered", InItem);
	}
}

void SFontBrowserTab::OnUnhoverFont(const FFontType& InItem)
{
	if (!SelectedFont.IsValid())
	{
		OnResetFont();
	}
}

FReply SFontBrowserTab::OnResetFont()
{
	SelectedFont.Reset();

	SelectedFontName->SetText(DefaultFontName);
	DetailFont->SetText(FText::FromString(""));
	DetailText->SetText(FText::FromString(FString::Format(*DefaultFontDetail, {FontAllList.Num()})));
	DemonstrateText->SetFont(FAppStyle::Get().GetFontStyle("NormalFont"));
	DemonstrateText->SetText(DemonstrateContent.IsEmpty() ? DefaultFontDemonstrate : DemonstrateContent);

	return FReply::Handled();
}

FReply SFontBrowserTab::OnSelectFont(const FString& InAction, const FFontType& InFont)
{
	SelectedFontName->SetText(FText::FromString(InFont->SimpleName));
	DetailFont->SetText(FText::FromName(InFont->TypefaceFontName));
	DetailFont->SetFont(InFont->Font);
	DetailText->SetText(FText::FromString(BuildDetail(InAction, InFont)));
	DemonstrateText->SetFont(InFont->Font);
	DemonstrateText->SetText(DemonstrateContent.IsEmpty() ? DefaultFontDemonstrate : DemonstrateContent);

	return FReply::Handled();
}

void SFontBrowserTab::OnDemonstrateChange(const FText& InText)
{
	DemonstrateContent = InText;
}
