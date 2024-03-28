
#include "ResourceBrowserTab.h"


void SResourceBrowserTab::Construct(const FArguments& InArgs)
{
	ResourceAllList = *FResourceInfo::CollectResources();

	ChildSlot
	[
		ConstructContent()
	];

	OnClickTabButton(ResourceAllList[0]);
}

TSharedRef<SWidget> SResourceBrowserTab::ConstructContent()
{
	return SNew(SVerticalBox)
		// tab button
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			[
				SAssignNew(ResourceTabView, SResourceTabView)
				.Orientation(EOrientation::Orient_Horizontal)
				.ListItemsSource(&ResourceAllList)
				.OnGenerateRow(this, &SResourceBrowserTab::OnGenerateTabButton)
			]
		]
		+SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SAssignNew(ResourceTabBox, SBox)
		];
}

TSharedRef<ITableRow> SResourceBrowserTab::OnGenerateTabButton(FResourceType InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<SButton> ItemButton = SNew(SButton)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Text(FText::FromString(InItem->TypeName))
		.OnClicked_Lambda([this, InItem]()
		{
			return this->OnClickTabButton(InItem);
		});

	ItemButton->SetEnabled(SelectedTab.IsValid() ? InItem->Type != SelectedTab->Type : true);
	TabButtonMap.Emplace(InItem->Type, ItemButton);

	return
		SNew(STableRow< TSharedPtr<FString> >, OwnerTable)
		[
			SNew(SBox)
			[
				ItemButton
			]
		];
}

FReply SResourceBrowserTab::OnClickTabButton(const FResourceType& InItem)
{
	SelectedTab = InItem;
	ResourceTabBox->SetContent(InItem->GetTabWidget());

	for(auto& Elem: TabButtonMap)
	{
		if (Elem.Key == InItem->Type)
		{
			Elem.Value->SetEnabled(false);
		}
		else
		{
			Elem.Value->SetEnabled(true);
		}
	}

	return FReply::Handled();
}
