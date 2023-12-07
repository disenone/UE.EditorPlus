#include "EditorPlusTab.h"

void FEditorPlusTab::Unregister()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabId);
}

void FEditorPlusTab::Register(FOnSpawnTab SpawnTabDelegate)
{
	FTabSpawnerEntry& Entry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabId, SpawnTabDelegate)
		.SetDisplayName(FText::FromName(TabId))
		.SetTooltipText(FText::FromName(Desc))
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
