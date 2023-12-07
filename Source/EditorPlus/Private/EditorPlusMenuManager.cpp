#include "EditorPlusMenuManager.h"
#include "EditorPlusPathMenuManager.h"

FEditorPlusMenuManager::FEditorPlusMenuManager()
{
	PathMenuManager = MakeShared<FEditorPlusPathMenuManager>();
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuManager::RegisterPath(const FName& Path, const FExecuteAction& ExecuteAction, const bool bMergeMenu)
{
	return Get()->PathMenuManager->Register(Path, ExecuteAction, bMergeMenu);
}


bool FEditorPlusMenuManager::UnregisterPath(const FName& Path, const TSharedRef<FEditorPlusMenuBase>& Command)
{
	return Get()->PathMenuManager->Unregister(Path, Command);
}

void FEditorPlusMenuManager::ExecutePathMenu(const FName& Path, FMenuBuilder& MenuBuilder, const TArray<FName>& MergedSubMenuNames, const bool bMerge)
{
	return Get()->PathMenuManager->ExecutePathMenu(Path, MenuBuilder, MergedSubMenuNames, bMerge);
}
