// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once
#include "Framework/Docking/TabManager.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Docking/SDockTab.h"
#include "EditorPlusUtils.h"

class EDITORPLUS_API FEditorPlusTab: public TSharedFromThis<FEditorPlusTab>
{
public:
	explicit FEditorPlusTab(
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty(), const TSharedPtr<FWorkspaceItem>& InGroup=nullptr)
		: TabId(FEditorPlusUtils::GenUniqueId("FEditorPlusTab")), FriendlyName(InFriendlyName), FriendlyTips(InFriendlyTips), Group(InGroup) {}

	virtual ~FEditorPlusTab() { FEditorPlusTab::Unregister(); }
	
	template <class TabWidgetClass>
	void Register()
	{
		FTabSpawnerEntry& Entry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			TabId,
			FOnSpawnTab::CreateSP(AsShared(), &FEditorPlusTab::SpawnTab<TabWidgetClass>))
			.SetDisplayName(FriendlyName)
			.SetTooltipText(FriendlyTips)
			.SetMenuType(ETabSpawnerMenuType::Hidden)
		;
			
		if (Group.IsValid())
		{
			Entry.SetGroup(Group.ToSharedRef());
		}
	}
	
	virtual void Register(const FOnSpawnTab& SpawnTabDelegate);
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
	FText FriendlyName;
	FText FriendlyTips;
	TSharedPtr<FWorkspaceItem> Group;
};
