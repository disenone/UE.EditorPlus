
#include "IconBrowserTab.h"
#include "EditorPlusUtils.h"
#include "EditorPlusToolsLog.h"

#include <regex>
#include <Widgets/Input/SSearchBox.h>

#include "WIdgets/Text/SMultiLineEditableText.h"

const static FString DefaultDetail("Click Icon to Select One\n[Total Num]: {0}");
const static FText DefaultIconName = FText::FromString("No Icon is Selected");

void SIconBrowserTab::Construct(const FArguments& InArgs)
{
	IconAllList = *FIconInfo::CollectIcons();

	ChildSlot
	[
		ConstructContent()
	];

}

TSharedRef<SWidget> SIconBrowserTab::ConstructContent()
{
	return SNew(SVerticalBox)
		// search
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			[
				SAssignNew(IconSearchBox, SSearchBox)
				.OnTextChanged(this, &SIconBrowserTab::OnSearchIcon)
			]
		]
		// icon tile view
		+ SVerticalBox::Slot()
		.FillHeight(0.8f)
		[
			SAssignNew(IconView, SIconView)
			.ListItemsSource(&IconAllList)
			.OnGenerateTile(this, &SIconBrowserTab::OnIconTile)

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
				// icon and reset button
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
									SAssignNew(DetailIcon, SImage)
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
								SAssignNew(SelectedIconName, SMultiLineEditableText)
								.AutoWrapText(true)
								.AllowMultiLine(true)
								.IsReadOnly(true)
								.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
								.Text(DefaultIconName)
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
								.OnClicked(this, &SIconBrowserTab::OnResetIcon)
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
						.Text(FText::FromString(FString::Format(*DefaultDetail, {IconAllList.Num()})))
					]
				]
			]]
		]
	;
}

TSharedRef<ITableRow> SIconBrowserTab::OnIconTile(FIconType InItem, const TSharedRef<STableViewBase>& OwnerTable)
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
						.Image(InItem->Icon.GetIcon())
						.DesiredSizeOverride(FVector2d(64, 64))
					]
				]
				.OnClicked_Lambda([this, InItem]()
				{
					return this->OnClickIcon(InItem);
				})
				.OnHovered_Lambda([this, InItem]()
				{
					return this->OnHoverIcon(InItem);
				})
				.OnUnhovered_Lambda([this, InItem]()
				{
					return this->OnUnhoverIcon(InItem);
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

void SIconBrowserTab::OnSearchIcon(const FText& InFilterText)
{
	if (IconSearchTimer.IsValid())
	{
		UnRegisterActiveTimer(IconSearchTimer.ToSharedRef());
	}

	IconSearchTimer = RegisterActiveTimer(IconSearchDelay, FWidgetActiveTimerDelegate::CreateLambda(
	[self = SharedThis(this)](double, double)
	{
		self->IconSearchTimer.Reset();
		self->UpdateIconList();
		return EActiveTimerReturnType::Stop;
	}));
}

void SIconBrowserTab::UpdateIconList()
{
	const FString FilterStr = IconSearchBox->GetText().ToString();

	if (FilterStr.IsEmpty())
	{
		IconView->SetItemsSource(&IconAllList);
	}
	else
	{
		IconList.Empty();
		IconView->SetItemsSource(&IconList);

		const FString PatternStr = TEXT("(.*?)(") + FString::Join(FEditorPlusUtils::SplitString(FilterStr, " "), TEXT(".*?")) + TEXT(")(.*)");
		const std::wregex Pattern = std::wregex(TCHAR_TO_WCHAR(ToCStr(PatternStr)), std::regex::icase);
		using FMatchItemType = TTuple<uint16, uint16, TSharedPtr<FIconInfo>>;
		TArray<FMatchItemType> MatchItems;

		for (TSharedRef<FIconInfo> Item: IconAllList)
		{
			std::wsmatch MatchResult;
			const std::wstring SearchText(TCHAR_TO_WCHAR(ToCStr(Item->FriendlyName)));
			if (!std::regex_match(SearchText, MatchResult, Pattern)) continue;

			if (MatchResult.empty() || !MatchResult.ready()) continue;

			MatchItems.Push(MakeTuple<uint16, uint16, TSharedPtr<FIconInfo>>(
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
				IconList.Push(MatchItem.Get<2>().ToSharedRef());
			}
		}
	}

	if (IconView.IsValid())
	{
		IconView->RebuildList();
		IconView->RequestListRefresh();
		IconView->ScrollToTop();
	}
}

FString BuildDetail(const FString& Action, const SIconBrowserTab::FIconType& Icon)
{
	FString Usage = Icon->Usage.Replace(TEXT("\n"), TEXT(" "));

	return FString::Format(
		TEXT(
			"[{0}]: {1}\n"
			"[Size]: {2} x {3}\n"
			"[Resource]: {5}\n"
			"[Usage]: {4}"),
		{
			Action,
			Icon->FriendlyName,
			static_cast<int>(Icon->Size.X),
			static_cast<int>(Icon->Size.Y),
			Usage,
			FPaths::ConvertRelativePathToFull(Icon->ResourceName.ToString())}
		);
}

FReply SIconBrowserTab::OnClickIcon(const FIconType& InItem)
{
	if (SelectedIcon == InItem)
	{
		return OnResetIcon();
	}

	SelectedIcon = InItem;
	return OnSelectIcon("Clicked", InItem);
}

void SIconBrowserTab::OnHoverIcon(const FIconType& InItem)
{
	if (!SelectedIcon.IsValid())
	{
		OnSelectIcon("Hovered", InItem);
	}
}

void SIconBrowserTab::OnUnhoverIcon(const FIconType& InItem)
{
	if (!SelectedIcon.IsValid())
	{
		OnResetIcon();
	}
}

FReply SIconBrowserTab::OnResetIcon()
{
	SelectedIcon.Reset();

	SelectedIconName->SetText(DefaultIconName);
	DetailIcon->SetImage(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Default").GetIcon());
	DetailText->SetText(FText::FromString(FString::Format(*DefaultDetail, {IconAllList.Num()})));

	return FReply::Handled();
}

FReply SIconBrowserTab::OnSelectIcon(const FString& InAction, const FIconType& InIcon)
{
	SelectedIconName->SetText(FText::FromString(InIcon->SimpleName));
	DetailIcon->SetImage(InIcon->Icon.GetIcon());
	DetailText->SetText(FText::FromString(BuildDetail(InAction, InIcon)));

	return FReply::Handled();
}
