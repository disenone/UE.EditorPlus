#pragma once
#include "Framework/Docking/TabManager.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Docking/SDockTab.h"

class EDITORPLUS_API FEditorPlusTab: public TSharedFromThis<FEditorPlusTab>
{
public:
	explicit FEditorPlusTab(const FName& TabId, const FName& DisplayName=NAME_None, const FName& Desc=NAME_None, const TSharedPtr<FWorkspaceItem> Group=nullptr)
		: TabId(TabId), DisplayName(DisplayName == NAME_None ? TabId : DisplayName), Desc(Desc), Group(Group) {}
	virtual ~FEditorPlusTab() { FEditorPlusTab::Unregister(); }
	
	template <class TabWidgetClass>
	void Register()
	{
		FTabSpawnerEntry& Entry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			TabId,
			FOnSpawnTab::CreateSP(AsShared(), &FEditorPlusTab::SpawnTab<TabWidgetClass>))
			.SetDisplayName(FText::FromName(TabId))
			.SetTooltipText(FText::FromName(Desc))
			.SetMenuType(ETabSpawnerMenuType::Hidden)
		;
			
		if (Group.IsValid())
		{
			Entry.SetGroup(Group.ToSharedRef());
		}
	}
	
	virtual void Register(FOnSpawnTab SpawnTabDelegate);
	virtual void Unregister();
	virtual void TryInvokeTab();
protected:
	template <class TabWidgetClass>
	TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& TabSpawnArgs)
	{
		TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				SNew(TabWidgetClass)
			];
		return SpawnedTab;
	}
	
	const FName TabId;
	const FName DisplayName;
	const FName Desc;
	TSharedPtr<FWorkspaceItem> Group;
};
