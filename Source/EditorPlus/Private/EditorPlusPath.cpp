#include "EditorPlusPath.h"
#include "EditorPlusMenuManager.h"

TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::RegisterPath(
	const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu)
{
	return FEditorPlusMenuManager::RegisterPath(Path, Menu);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::RegisterPath(const FString& Path, const FText& FriendlyName, const FText& FriendlyTips)
{
	return FEditorPlusMenuManager::RegisterPath(Path, FriendlyName, FriendlyTips);
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::RegisterPathAction(
	const FString& Path, const FExecuteAction& ExecuteAction, const FName& Hook, const FText& FriendlyName, const FText& FriendlyTips)
{
	return FEditorPlusMenuManager::RegisterPathAction(Path, ExecuteAction, Hook, FriendlyName, FriendlyTips);
}

bool FEditorPlusPath::UnregisterPath(
	const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Leaf)
{
	return FEditorPlusMenuManager::UnregisterPath(Path, Leaf);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::GetNodeByPath(const FString& Path)
{
	return FEditorPlusMenuManager::GetNodeByPath(Path);
}
