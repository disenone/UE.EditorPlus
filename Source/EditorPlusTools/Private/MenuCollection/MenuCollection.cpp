// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#include "MenuCollection.h"
#include "EditorPlusPath.h"
#include "EditorPlusUtils.h"
#include <regex>

#include "MenuItem.h"
#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY(LogMenuCollection);

#define LOCTEXT_NAMESPACE "EditorPlusTools"


FMenuCollection::FMenuCollection()
{
	InitIcon();
}


void FMenuCollection::OnStartup()
{
	BuildMenu();
}

void FMenuCollection::OnShutdown()
{
	if (!Menu.IsValid()) return;

	Menu->Destroy();
	Menu.Reset();
}

void FMenuCollection::BuildMenu()
{
	if (Menu.IsValid()) return;

	LoadMenuItemsConfig();
	// <Section> MenuCollection
	// | Find
	// | Collections

	// <Section> MenuCollection
	Menu = FEditorPlusPath::RegisterPath("/<MenuBar>EditorPlusTools/<Section>MenuCollection");
	Menu->SetFriendlyName(LOCTEXT("MenuCollection", "MenuCollection"));
	Menu->SubscribePreBuildMenu(FMenuExtensionDelegate::CreateSP(this, &FMenuCollection::OnPreShowMenu));

	// | Find
	FEditorPlusPath::RegisterChildPath(Menu.ToSharedRef(), "<SubMenu>Find",
		EP_NEW_MENU(FEditorPlusSubMenu)("MenuCollectionFind", "MenuCollectionFind", LOCTEXT("MenuCollectionFind", "Find"))
		->Content({
			EP_NEW_MENU(FEditorPlusWidget)("MenuCollectionFind")
			->BindWidget(
				SNew(SBox)
				.WidthOverride(400)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.MaxHeight(40)
					[
						SNew(SBox)
						[
							SAssignNew(MenuSearchBox, SSearchBox)
							.MinDesiredWidth(300)
							.OnTextChanged(SharedThis(this), &FMenuCollection::OnMenuNameFilterChanged)
						]
					]
					+ SVerticalBox::Slot()
					.MaxHeight(400)
					[
						SNew(SBox)
						[
							SAssignNew(MenuSearchListView, SMenuNameListView)
							.ListItemsSource(&MenuSearchItems)
							.OnGenerateRow(SharedThis(this), &FMenuCollection::OnGenerateWidgetForMenuSearchListView)
						]
					]
				]
			)
		})
	);

	// | Collections
	FEditorPlusPath::RegisterChildPath(Menu.ToSharedRef(), "<SubMenu>Collections",
		EP_NEW_MENU(FEditorPlusSubMenu)("MenuCollections", "MenuCollections", LOCTEXT("MenuCollections", "Collections"))
		->Content({
			EP_NEW_MENU(FEditorPlusWidget)("MenuCollections")
			->BindWidget(
				SNew(SBox)
				.WidthOverride(400)
				[
					SAssignNew(MenuSavedListView, SMenuNameListView)
					.ListItemsSource(&MenuSavedItems)
					.OnGenerateRow(SharedThis(this), &FMenuCollection::OnGenerateWidgetForMenuSearchListView)
				]
			)
		})
	);
}

void FMenuCollection::OnPreShowMenu(FMenuBuilder& MenuBuilder)
{
	MenuListCache = IMenuItem::CollectMenuItems();
	MenuMapCache.Reset();
	for (const auto Item: MenuListCache)
	{
		MenuMapCache.Add(Item->GetSaveString(), Item);
	}
	MenuSearchBox->SetText(FText::GetEmpty());
	MenuSearchItems = MenuListCache;

	MenuSavedItems.Reset();
	if (!MenuSavedConfig.IsEmpty())
	{
		for (const auto ItemString: MenuSavedConfig)
		{
			auto Item = MenuMapCache.Find(ItemString);
			if (Item)
			{
				MenuSavedItems.Push(*Item);
			}
		}
	}

	UpdateMenuSearchItems();
	if (MenuSavedListView.IsValid())
	{
		MenuSavedListView->RebuildList();
		MenuSavedListView->RequestListRefresh();
		MenuSavedListView->ScrollToTop();
	}
	UE_LOG(LogMenuCollection, Display, TEXT("MenuCollection Collected"));
}

void FMenuCollection::OnMenuNameFilterChanged(const FText& InFilterText)
{
	if (MenuSearchTimer.IsValid())
	{
		MenuSearchBox->UnRegisterActiveTimer(MenuSearchTimer.ToSharedRef());
	}

	MenuSearchTimer = MenuSearchBox->RegisterActiveTimer(
		MenuSearchDelay,
		FWidgetActiveTimerDelegate::CreateLambda(
		[self = SharedThis(this)](double, double)
		{
			self->MenuSearchTimer.Reset();
			self->UpdateMenuSearchItems();
			return EActiveTimerReturnType::Stop;
		}));
}


void FMenuCollection::UpdateMenuSearchItems()
{
	MenuSearchItems.Reset();

	const FString FilterStr = MenuSearchBox->GetText().ToString();

	if (!FilterStr.IsEmpty())
	{
		// const FString PatternStr = TEXT("(.*?)(") + FString::Join(FilterStr, TEXT(".*?")) + TEXT(")(.*)");
		const FString PatternStr = TEXT("(.*?)(") + FString::Join(FEditorPlusUtils::SplitString(FilterStr, " "), TEXT(".*?")) + TEXT(")(.*)");
		UE_LOG(LogMenuCollection, Display, TEXT("MenuCollection Search Pattern: %s, %llu, %llu"), *PatternStr, sizeof(TCHAR), sizeof(wchar_t));
		const std::wregex Pattern = std::wregex(TCHAR_TO_WCHAR(ToCStr(PatternStr)), std::regex::icase);

		using FMatchItemType = TTuple<uint16, uint16, TSharedPtr<IMenuItem>>;
		TArray<FMatchItemType> MatchItems;

		for (TSharedPtr<IMenuItem> Item: MenuListCache)
		{
			std::wsmatch MatchResult;
			const std::wstring SearchText(TCHAR_TO_WCHAR(*Item->GetSearchText()));
			if (!std::regex_match(SearchText, MatchResult, Pattern)) continue;

			if (MatchResult.empty() || !MatchResult.ready()) continue;

			MatchItems.Push(MakeTuple<uint16, uint16, TSharedPtr<IMenuItem>>(
				 MatchResult[2].length(), MatchResult[1].length(), Item.ToSharedRef()));

			UE_LOG(
				LogMenuCollection, Display, TEXT("MenuCollection Search Match: %s, %s, %s, %s"),
				WCHAR_TO_TCHAR(MatchResult[1].str().c_str()), WCHAR_TO_TCHAR(MatchResult[2].str().c_str()),
				WCHAR_TO_TCHAR(MatchResult[3].str().c_str()), WCHAR_TO_TCHAR(TCHAR_TO_WCHAR(*Item->GetSearchText())));
		}

		if (!MatchItems.IsEmpty())
		{
			MatchItems.Sort([](FMatchItemType a, FMatchItemType b)
			{
				if (a.Get<0>() != b.Get<0>())
				{
					return a.Get<0>() < b.Get<0>();
				}

				if (a.Get<1>() != b.Get<1>())
				{
					return a.Get<1>() < b.Get<1>();
				}
				return a.Get<2>()->GetName().Compare(b.Get<2>()->GetName()) < 0;
			});
			for (FMatchItemType& MatchItem: MatchItems)
			{
				MenuSearchItems.Push(MatchItem.Get<2>());
			}
		}
	}
	else
	{
		MenuSearchItems = MenuListCache;
	}

	if (MenuSearchListView.IsValid())
	{
		MenuSearchListView->RebuildList();
		MenuSearchListView->RequestListRefresh();
		MenuSearchListView->ScrollToTop();
	}
}

TSharedRef<ITableRow> FMenuCollection::OnGenerateWidgetForMenuSearchListView(
	TSharedPtr<IMenuItem> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{

	class SMenuItemWidget: public STableRow<TSharedPtr<IMenuItem>>
	{
	public:
		SLATE_BEGIN_ARGS(SMenuItemWidget){}
		SLATE_END_ARGS()
		using FSuperRowType = STableRow;

		void Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable,
			const TSharedPtr<IMenuItem>& InListItem, const TSharedRef<FMenuCollection>& ThisRef)
		{
			Item = InListItem;
			FSuperRowType::Construct(
				FSuperRowType::FArguments()
				.Padding(FMargin(2, 2))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.FillWidth(1.0)
					[
						SNew(SBox)
						[
							SNew(SButton)
							.VAlign(VAlign_Center)
							.Text(Item->GetFriendlyName())
							.ToolTipText(Item->GetFriendlyTips())
							.OnClicked(FOnClicked::CreateLambda(
								[ItemRef=Item.ToSharedRef()]
								{
									if (!ItemRef->RunAction())
									{
										UE_LOG(LogMenuCollection, Error, TEXT("Failed to run [%s]"), ToCStr(ItemRef->GetFriendlyName().ToString()));
									}
									return FReply::Handled();
								}))
						]
					]
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Right)
					.AutoWidth()
					[
						SNew(SBox)
						[
							SNew(SButton)
							[
								SAssignNew(StarImage, SImage)
								.Image_Lambda([ThisRef, ItemRef=Item.ToSharedRef()]
								{
									if(ThisRef->MenuSavedItems.Contains(ItemRef))
									{
										return ThisRef->StaredIcon.Get();
									}
									return ThisRef->StarIcon.Get();
								})
							]
							.OnClicked(FOnClicked::CreateLambda(
								[ThisRef, ItemRef=Item.ToSharedRef()]
								{
									if(ThisRef->MenuSavedItems.Contains(ItemRef))
									{
										ThisRef->MenuSavedItems.RemoveSingle(ItemRef);
										ThisRef->MenuSavedConfig.Remove(ItemRef->GetSaveString());
									}
									else
									{
										ThisRef->MenuSavedItems.Push(ItemRef);
										ThisRef->MenuSavedConfig.Push(ItemRef->GetSaveString());
									}
									if (ThisRef->MenuSavedListView.IsValid())
									{
										ThisRef->MenuSavedListView->RebuildList();
										ThisRef->MenuSavedListView->RequestListRefresh();
										ThisRef->MenuSavedListView->ScrollToTop();
									}
									ThisRef->SaveMenuItemsConfig();
									return FReply::Handled();
								}))
						]
					]
				]
				, InOwnerTable);
		}

	private:
		TSharedPtr<IMenuItem> Item;
		TSharedPtr<SImage> StarImage;
	};

	return SNew(SMenuItemWidget, OwnerTable, InItem, SharedThis(this));
}

void FMenuCollection::InitIcon()
{
	if(StarIcon.IsValid()) return;

	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("EditorPlus"));
	check(Plugin.IsValid());
	const auto ResourcesPath = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources"));

	const FVector2D Icon16x16(16.0f, 16.0f);
	StarIcon = MakeShared<FSlateVectorImageBrush>(FPaths::Combine(ResourcesPath, TEXT("save.svg")), Icon16x16);
	StaredIcon = MakeShared<FSlateVectorImageBrush>(FPaths::Combine(ResourcesPath, TEXT("delete.svg")), Icon16x16);
}

static const FString GConfigFilePath = IPluginManager::Get().FindPlugin("EditorPlus")->GetBaseDir() / TEXT("Config") / TEXT("MenuCollection.ini");

void FMenuCollection::LoadMenuItemsConfig()
{
	if (FPaths::FileExists(GConfigFilePath))
	{
		UE_LOG(LogMenuCollection, Display, TEXT("MenuCollection Config File: %s"), *GConfigFilePath);
	}
	else
	{
		UE_LOG(LogMenuCollection, Display, TEXT("MenuCollection Config File Not Found: %s"), *GConfigFilePath);
		return;
	}

	FConfigFile Config;
	Config.Read(GConfigFilePath);
	Config.GetArray(TEXT("Collections"), TEXT("Menu"), MenuSavedConfig);
}


void FMenuCollection::SaveMenuItemsConfig() const
{
	FConfigFile Config;
	Config.Empty();
	Config.SetArray(TEXT("Collections"), TEXT("Menu"), MenuSavedConfig);
	Config.Write(GConfigFilePath);
}

#undef LOCTEXT_NAMESPACE
