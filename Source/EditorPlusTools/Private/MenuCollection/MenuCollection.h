#pragma once

#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"
#include "Slate.h"

class IMenuItem;

class FMenuCollection final: public IEditorPlusToolInterface
{
public:
	FMenuCollection();
	virtual void OnStartup() override;
	virtual void OnShutdown() override;

private:
	void BuildMenu();
	void OnPreShowMenu(FMenuBuilder& MenuBuilder);

	void OnMenuNameFilterChanged(const FText& InFilterText);
	TSharedRef<ITableRow> OnGenerateWidgetForMenuSearchListView(TSharedPtr<IMenuItem> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void UpdateMenuSearchItems();
	void InitIcon();
	void SaveMenuItemsConfig();

	typedef SListView<TSharedPtr<IMenuItem>> SMenuNameListView;

	TSharedPtr<FEditorPlusMenuBase> Menu;

	TArray<TSharedPtr<IMenuItem>> MenuListCache;
	TArray<TSharedPtr<IMenuItem>> MenuSearchItems;
	TSharedPtr<SSearchBox>	MenuSearchBox;
	TSharedPtr<SMenuNameListView> MenuSearchListView;
	TSharedPtr<FActiveTimerHandle> MenuSearchTimer;
	const float MenuSearchDelay = 0.1f;
	TSharedPtr<FSlateImageBrush> StarIcon;
	TSharedPtr<FSlateImageBrush> StaredIcon;

	TSharedPtr<SMenuNameListView> MenuSavedListView;
	TArray<TSharedPtr<IMenuItem>> MenuSavedItems;
};

DECLARE_LOG_CATEGORY_EXTERN(LogMenuCollection, Display, All);
