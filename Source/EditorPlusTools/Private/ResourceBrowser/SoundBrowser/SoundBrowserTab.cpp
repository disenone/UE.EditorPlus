
#include "SoundBrowserTab.h"
#include "EditorPlusUtils.h"
#include "EditorPlusToolsLog.h"

#include <regex>
#include <Widgets/Input/SSearchBox.h>

#include "WIdgets/Text/SMultiLineEditableText.h"

const static FString DefaultSoundDetail("Click Sound to Select And Play One\n[Total Num]: {0}");
const static FText DefaultSoundName = FText::FromString("No Sound is Selected");

void SSoundBrowserTab::Construct(const FArguments& InArgs)
{
	SoundAllList = *FSoundInfo::CollectSounds();

	ChildSlot
	[
		ConstructContent()
	];

}

TSharedRef<SWidget> SSoundBrowserTab::ConstructContent()
{
	return SNew(SVerticalBox)
		// search
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			[
				SAssignNew(SoundSearchBox, SSearchBox)
				.OnTextChanged(this, &SSoundBrowserTab::OnSearchSound)
			]
		]
		// Sound tile view
		+ SVerticalBox::Slot()
		.FillHeight(0.8f)
		[
			SAssignNew(SoundView, SSoundView)
			.ListItemsSource(&SoundAllList)
			.OnGenerateTile(this, &SSoundBrowserTab::OnSoundTile)

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
				// Sound and reset button
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
								SNew(SBox)
								.HeightOverride(64)
								.WidthOverride(64)
								[
									SNew(SButton)
									.OnClicked(this, &SSoundBrowserTab::OnPlaySound)
									[
										SAssignNew(SelectSoundText, STextBlock)
										.Text(FText::GetEmpty())
										.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
										.AutoWrapText(true)
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
								SAssignNew(SelectedSoundName, SMultiLineEditableText)
								.AutoWrapText(true)
								.AllowMultiLine(true)
								.IsReadOnly(true)
								.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
								.Text(DefaultSoundName)
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
								.OnClicked(this, &SSoundBrowserTab::OnResetSound)
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
						.Text(FText::FromString(FString::Format(*DefaultSoundDetail, {SoundAllList.Num()})))
					]
				]
			]]
		]
	;
}

TSharedRef<ITableRow> SSoundBrowserTab::OnSoundTile(FSoundType InItem, const TSharedRef<STableViewBase>& OwnerTable)
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
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString(InItem->SimpleName))
							.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
							.AutoWrapText(true)
						]
					]
				]
				.OnClicked_Lambda([this, InItem]()
				{
					return this->OnClickSound(InItem);
				})
				.OnHovered_Lambda([this, InItem]()
				{
					return this->OnHoverSound(InItem);
				})
				.OnUnhovered_Lambda([this, InItem]()
				{
					return this->OnUnhoverSound(InItem);
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
				.Text(FText::FromString(InItem->FriendlyName))
			]
		];
}

void SSoundBrowserTab::OnSearchSound(const FText& InFilterText)
{
	if (SoundSearchTimer.IsValid())
	{
		UnRegisterActiveTimer(SoundSearchTimer.ToSharedRef());
	}

	SoundSearchTimer = RegisterActiveTimer(SoundSearchDelay, FWidgetActiveTimerDelegate::CreateLambda(
	[self = SharedThis(this)](double, double)
	{
		self->SoundSearchTimer.Reset();
		self->UpdateSoundList();
		return EActiveTimerReturnType::Stop;
	}));
}

void SSoundBrowserTab::UpdateSoundList()
{
	const FString FilterStr = SoundSearchBox->GetText().ToString();

	if (FilterStr.IsEmpty())
	{
		SoundView->SetItemsSource(&SoundAllList);
	}
	else
	{
		SoundList.Empty();
		SoundView->SetItemsSource(&SoundList);

		const FString PatternStr = TEXT("(.*?)(") + FString::Join(FEditorPlusUtils::SplitString(FilterStr, " "), TEXT(".*?")) + TEXT(")(.*)");
		const std::wregex Pattern = std::wregex(TCHAR_TO_WCHAR(ToCStr(PatternStr)), std::regex::icase);
		using FMatchItemType = TTuple<uint16, uint16, TSharedPtr<FSoundInfo>>;
		TArray<FMatchItemType> MatchItems;

		for (TSharedRef<FSoundInfo> Item: SoundAllList)
		{
			std::wsmatch MatchResult;
			const std::wstring SearchText(TCHAR_TO_WCHAR(ToCStr(Item->FriendlyName)));
			if (!std::regex_match(SearchText, MatchResult, Pattern)) continue;

			if (MatchResult.empty() || !MatchResult.ready()) continue;

			MatchItems.Push(MakeTuple<uint16, uint16, TSharedPtr<FSoundInfo>>(
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
				SoundList.Push(MatchItem.Get<2>().ToSharedRef());
			}
		}
	}

	if (SoundView.IsValid())
	{
		SoundView->RebuildList();
		SoundView->RequestListRefresh();
		SoundView->ScrollToTop();
	}
}

FString BuildDetail(const FString& Action, const SSoundBrowserTab::FSoundType& Sound)
{
	FString Usage = Sound->Usage.Replace(TEXT("\n"), TEXT(" "));

	return FString::Format(
		TEXT(
			"[{0}]: {1}\n"
			"[Usage]: {2}"),
		{
			Action,
			Sound->FriendlyName,
			Usage,
		});
}

FReply SSoundBrowserTab::OnClickSound(const FSoundType& InItem)
{
	if (SelectedSound == InItem)
	{
		return OnResetSound();
	}

	SelectedSound = InItem;
	OnSelectSound("Clicked", InItem);
	return OnPlaySound();
}

void SSoundBrowserTab::OnHoverSound(const FSoundType& InItem)
{
	if (!SelectedSound.IsValid())
	{
		OnSelectSound("Hovered", InItem);
	}
}

void SSoundBrowserTab::OnUnhoverSound(const FSoundType& InItem)
{
	if (!SelectedSound.IsValid())
	{
		OnResetSound();
	}
}

FReply SSoundBrowserTab::OnResetSound()
{
	SelectedSound.Reset();

	SelectedSoundName->SetText(DefaultSoundName);
	SelectSoundText->SetText(FText::GetEmpty());
	DetailText->SetText(FText::FromString(FString::Format(*DefaultSoundDetail, {SoundAllList.Num()})));

	return FReply::Handled();
}

FReply SSoundBrowserTab::OnSelectSound(const FString& InAction, const FSoundType& InSound)
{
	SelectedSoundName->SetText(FText::FromString(InSound->FriendlyName));
	SelectSoundText->SetText(FText::FromString(InSound->SimpleName));
	DetailText->SetText(FText::FromString(BuildDetail(InAction, InSound)));

	return FReply::Handled();
}

FReply SSoundBrowserTab::OnPlaySound()
{
	if (SelectedSound.IsValid())
	{
		FSlateApplication::Get().PlaySound(SelectedSound->Sound);
	}

	return FReply::Handled();
}
