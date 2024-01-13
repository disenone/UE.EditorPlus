#include "EditorPlusTab.h"

void FEditorPlusTab::Unregister()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabId);
}

void FEditorPlusTab::Register(const FOnSpawnTab& SpawnTabDelegate)
{
	FTabSpawnerEntry& Entry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabId, SpawnTabDelegate)
		.SetDisplayName(FriendlyName)
		.SetTooltipText(FriendlyTips)
		.SetMenuType(ETabSpawnerMenuType::Hidden)
	;
	
	if (Group.IsValid())
	{
		Entry.SetGroup(Group.ToSharedRef());
	}
}

void FEditorPlusTab::TryInvokeTab()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TabId);
}
